//
// Created by AmazingBuff on 2025/4/25.
//

#include "dx12graphics_pipeline.h"
#include "dx12device.h"
#include "dx12root_signature.h"
#include "dx12.h"
#include "rendering/api.h"
#include "resources/dx12shader_library.h"
#include "utils/dx_macro.h"
#include "utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN


DX12GraphicsPipeline::DX12GraphicsPipeline(GPUDevice const* device, GPUGraphicsPipelineCreateInfo const& info) : m_root_signature(nullptr), m_pipeline_state(nullptr), m_pipeline_state_desc{}, m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    DX12RootSignature const* dx12_root_signature = static_cast<DX12RootSignature const*>(info.root_signature);

    uint32_t input_element_count = 0;
    Vector<D3D12_INPUT_ELEMENT_DESC> input_element_desc;
    if (!info.vertex_input.empty())
    {
        for (GPUVertexAttribute const& vertex_attribute : info.vertex_input)
            input_element_count += vertex_attribute.array_size;

        input_element_desc.resize(input_element_count);

        Map<String, uint32_t> semantic_name_to_index;
        uint32_t fill_index = 0;
        for (GPUVertexAttribute const& vertex_attribute : info.vertex_input)
        {
            for (uint32_t i = 0; i < vertex_attribute.array_size; ++i)
            {
                input_element_desc[fill_index].SemanticName = vertex_attribute.semantic_name.c_str();
                auto iter = semantic_name_to_index.find(vertex_attribute.semantic_name);
                if (iter != semantic_name_to_index.end())
                    iter->second++;
                else
                {
                    semantic_name_to_index.emplace(vertex_attribute.semantic_name, 0);
                    iter = semantic_name_to_index.find(vertex_attribute.semantic_name);
                }

                input_element_desc[fill_index].SemanticIndex = iter->second;
                input_element_desc[fill_index].Format = transfer_format(vertex_attribute.format);
                input_element_desc[fill_index].InputSlot = vertex_attribute.binding;
                input_element_desc[fill_index].AlignedByteOffset = vertex_attribute.offset + i * format_bit_size(vertex_attribute.format) / 8;
                if (vertex_attribute.rate == GPUVertexInputRate::e_instance)
                {
                    input_element_desc[fill_index].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
                    input_element_desc[fill_index].InstanceDataStepRate = 1;
                }
                else
                {
                    input_element_desc[fill_index].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    input_element_desc[fill_index].InstanceDataStepRate = 1;
                }
                fill_index++;
            }
        }
    }
    D3D12_INPUT_LAYOUT_DESC input_layout_desc{
        .pInputElementDescs = input_element_desc.data(),
        .NumElements = input_element_count,
    };

    // shader stages
#define SHADER_STAGE(stage)                                                                                             \
    D3D12_SHADER_BYTECODE stage{};                                                                                      \
    if (info.stage)                                                                                                     \
    {                                                                                                                   \
        DX12ShaderLibrary const* dx12_shader_library = static_cast<DX12ShaderLibrary const*>(info.stage->library);      \
        (stage).BytecodeLength = dx12_shader_library->m_blob_encoding->GetBufferSize();                                 \
        (stage).pShaderBytecode = dx12_shader_library->m_blob_encoding->GetBufferPointer();                             \
    }

    SHADER_STAGE(vertex_shader);
    SHADER_STAGE(tessellation_control_shader);
    SHADER_STAGE(tessellation_evaluation_shader);
    SHADER_STAGE(geometry_shader);
    SHADER_STAGE(fragment_shader);
#undef SHADER_STAGE

    // stream out
    D3D12_STREAM_OUTPUT_DESC stream_output_desc{
        .pSODeclaration = nullptr,
        .NumEntries = 0,
        .pBufferStrides = nullptr,
        .NumStrides = 0,
        .RasterizedStream = 0
    };

    // sample
    DXGI_SAMPLE_DESC sample_desc{
        .Count = static_cast<uint32_t>(info.sample_count > GPUSampleCount::e_1 ? to_underlying(info.sample_count) : 1),
        .Quality = info.sample_quality,
    };
    D3D12_CACHED_PIPELINE_STATE cached_pipeline_state{
        .pCachedBlob = nullptr,
        .CachedBlobSizeInBytes = 0,
    };

    // blend state
    D3D12_BLEND_DESC blend_desc{
        .IndependentBlendEnable = TRUE,
    };

    if (info.blend_state)
    {
        GPUBlendState const* blend_state = info.blend_state;
        blend_desc.AlphaToCoverageEnable = static_cast<uint32_t>(info.blend_state->alpha_to_coverage);

        uint32_t blend_index = 0;
        for (uint32_t i = 0; i < GPU_Max_Render_Target; i++)
        {
            D3D12_RENDER_TARGET_BLEND_DESC& rt_blend = blend_desc.RenderTarget[i];
            rt_blend.SrcBlend = Blend_Constant_Map[to_underlying(blend_state->src_factors[blend_index])];
            rt_blend.DestBlend = Blend_Constant_Map[to_underlying(blend_state->dst_factors[blend_index])];
            rt_blend.SrcBlendAlpha = Blend_Constant_Map[to_underlying(blend_state->alpha_src_factors[blend_index])];
            rt_blend.DestBlendAlpha = Blend_Constant_Map[to_underlying(blend_state->alpha_dst_factors[blend_index])];

            if (rt_blend.SrcBlend != D3D12_BLEND_ONE || rt_blend.DestBlend != D3D12_BLEND_ZERO ||
                rt_blend.SrcBlendAlpha != D3D12_BLEND_ONE || rt_blend.DestBlendAlpha != D3D12_BLEND_ZERO)
                rt_blend.BlendEnable = 1;
            else
                rt_blend.BlendEnable = 0;

            rt_blend.RenderTargetWriteMask = blend_state->write_masks[blend_index];
            rt_blend.BlendOp = Blend_Op_Map[to_underlying(blend_state->blend_modes[blend_index])];
            rt_blend.BlendOpAlpha = Blend_Op_Map[to_underlying(blend_state->alpha_blend_modes[blend_index])];

            if (blend_state->independent_blend)
                blend_index++;
        }
    }
    else
    {
        blend_desc.AlphaToCoverageEnable = FALSE;

        for (uint32_t i = 0; i < GPU_Max_Render_Target; i++)
        {
            D3D12_RENDER_TARGET_BLEND_DESC& rt_blend = blend_desc.RenderTarget[i];
            rt_blend.BlendEnable = FALSE;
            rt_blend.LogicOpEnable = FALSE;
            rt_blend.SrcBlend = D3D12_BLEND_ONE;
            rt_blend.DestBlend = D3D12_BLEND_ZERO;
            rt_blend.BlendOp = D3D12_BLEND_OP_ADD;
            rt_blend.SrcBlendAlpha = D3D12_BLEND_ONE;
            rt_blend.DestBlendAlpha = D3D12_BLEND_ZERO;
            rt_blend.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            rt_blend.LogicOp = D3D12_LOGIC_OP_CLEAR;
            rt_blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }
    }

    // rasterizer state
    D3D12_RASTERIZER_DESC rasterizer_desc{
        .FillMode = D3D12_FILL_MODE_SOLID,
        .CullMode = D3D12_CULL_MODE_BACK,
        .FrontCounterClockwise = 1,
        .DepthBias = 0,
        .DepthBiasClamp = 0.0f,
        .SlopeScaledDepthBias = 0.0f,
        .DepthClipEnable = 1,
        .MultisampleEnable = 0,
        .AntialiasedLineEnable = 0,
        .ForcedSampleCount = 0,
        .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
    };
    if (info.rasterizer_state)
    {
        GPURasterizerState const* rasterizer_state = info.rasterizer_state;
        rasterizer_desc.FillMode = Fill_Mode_Map[to_underlying(rasterizer_state->fill_mode)];
        rasterizer_desc.CullMode = Cull_Mode_Map[to_underlying(rasterizer_state->cull_mode)];
        rasterizer_desc.FrontCounterClockwise = rasterizer_state->front_face == GPUFrontFace::e_counter_clockwise;
        rasterizer_desc.DepthBias = rasterizer_state->depth_bias;
        rasterizer_desc.DepthBiasClamp = 0.0f;
        rasterizer_desc.SlopeScaledDepthBias = rasterizer_state->slope_scaled_depth_bias;
        rasterizer_desc.DepthClipEnable = rasterizer_state->enable_depth_clamp;
        rasterizer_desc.MultisampleEnable = rasterizer_state->enable_multisample;
        rasterizer_desc.AntialiasedLineEnable = 0;
        rasterizer_desc.ForcedSampleCount = 0;
        rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    }

    // depth stencil state
    D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{
        .DepthEnable = FALSE,
        .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO,
        .DepthFunc = D3D12_COMPARISON_FUNC_NEVER,
        .StencilEnable = FALSE,
        .StencilReadMask = 0,
        .StencilWriteMask = 0,
        .FrontFace {
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_NEVER,
        },
        .BackFace{
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_NEVER,
        },
    };
    if (info.depth_stencil_state)
    {
        GPUDepthStencilState const* depth_stencil_state = info.depth_stencil_state;
        depth_stencil_desc.DepthEnable = depth_stencil_state->depth_test;
        depth_stencil_desc.DepthWriteMask = depth_stencil_state->depth_write ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        depth_stencil_desc.DepthFunc = Compare_Mode_Map[to_underlying(depth_stencil_state->depth_compare)];
        depth_stencil_desc.StencilEnable = depth_stencil_state->stencil_test;
        depth_stencil_desc.StencilReadMask = depth_stencil_state->stencil_read_mask;
        depth_stencil_desc.StencilWriteMask = depth_stencil_state->stencil_write_mask;
        depth_stencil_desc.FrontFace.StencilFunc = Compare_Mode_Map[to_underlying(depth_stencil_state->stencil_front_compare)];
        depth_stencil_desc.FrontFace.StencilDepthFailOp = Stencil_Op_Map[to_underlying(depth_stencil_state->depth_front_fail)];
        depth_stencil_desc.FrontFace.StencilFailOp = Stencil_Op_Map[to_underlying(depth_stencil_state->stencil_front_fail)];
        depth_stencil_desc.FrontFace.StencilPassOp = Stencil_Op_Map[to_underlying(depth_stencil_state->stencil_front_pass)];
        depth_stencil_desc.BackFace.StencilFunc = Compare_Mode_Map[to_underlying(depth_stencil_state->stencil_back_compare)];
        depth_stencil_desc.BackFace.StencilDepthFailOp = Stencil_Op_Map[to_underlying(depth_stencil_state->depth_back_fail)];
        depth_stencil_desc.BackFace.StencilFailOp = Stencil_Op_Map[to_underlying(depth_stencil_state->stencil_back_fail)];
        depth_stencil_desc.BackFace.StencilPassOp = Stencil_Op_Map[to_underlying(depth_stencil_state->stencil_back_pass)];
    }

    // pso
    m_pipeline_state_desc.pRootSignature = dx12_root_signature->m_root_signature;
    // single gpu
    m_pipeline_state_desc.NodeMask = GPU_Node_Mask;
    m_pipeline_state_desc.VS = vertex_shader;
    m_pipeline_state_desc.HS = tessellation_control_shader;
    m_pipeline_state_desc.DS = tessellation_evaluation_shader;
    m_pipeline_state_desc.GS = geometry_shader;
    m_pipeline_state_desc.PS = fragment_shader;
    m_pipeline_state_desc.StreamOutput = stream_output_desc;
    m_pipeline_state_desc.BlendState = blend_desc;
    m_pipeline_state_desc.SampleMask = std::numeric_limits<uint32_t>::max();
    m_pipeline_state_desc.RasterizerState = rasterizer_desc;
    m_pipeline_state_desc.DepthStencilState = depth_stencil_desc;
    m_pipeline_state_desc.InputLayout = input_layout_desc;
    m_pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    m_pipeline_state_desc.PrimitiveTopologyType = transfer_primitive_topology(info.primitive_topology);
    m_pipeline_state_desc.NumRenderTargets = info.render_target_count;
    m_pipeline_state_desc.DSVFormat = transfer_format(info.depth_stencil_format);
    m_pipeline_state_desc.SampleDesc = sample_desc;
    m_pipeline_state_desc.CachedPSO = cached_pipeline_state;
    m_pipeline_state_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    for (uint32_t i = 0; i < info.render_target_count; i++)
        m_pipeline_state_desc.RTVFormats[i] = transfer_format(info.color_format[i]);

    // create
    HRESULT result = E_FAIL;
    wchar_t pipeline_name[DX12_Pipeline_State_Object_Name_Max_Length]{};
    uint64_t shader_hash = Rendering_Hash;
    uint64_t graphics_hash = Rendering_Hash;
    if (dx12_device->m_pipeline_library)
    {
#define SHADER_HASH(shader) if ((shader).BytecodeLength) shader_hash = hash_combine(shader_hash, shader.pShaderBytecode, shader.BytecodeLength);
        SHADER_HASH(vertex_shader);
        SHADER_HASH(tessellation_control_shader);
        SHADER_HASH(tessellation_evaluation_shader);
        SHADER_HASH(geometry_shader);
        SHADER_HASH(fragment_shader);
#undef SHADER_HASH

        graphics_hash = hash_combine(graphics_hash, &m_pipeline_state_desc.BlendState, sizeof(D3D12_BLEND_DESC));
        graphics_hash = hash_combine(graphics_hash, &m_pipeline_state_desc.RasterizerState, sizeof(D3D12_RASTERIZER_DESC));
        graphics_hash = hash_combine(graphics_hash, &m_pipeline_state_desc.DepthStencilState, sizeof(D3D12_DEPTH_STENCIL_DESC));
        graphics_hash = hash_combine(graphics_hash, &m_pipeline_state_desc.IBStripCutValue, sizeof(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE));
        graphics_hash = hash_combine(graphics_hash, m_pipeline_state_desc.RTVFormats, m_pipeline_state_desc.NumRenderTargets * sizeof(DXGI_FORMAT));
        graphics_hash = hash_combine(graphics_hash, &m_pipeline_state_desc.DSVFormat, sizeof(DXGI_FORMAT));
        graphics_hash = hash_combine(graphics_hash, &m_pipeline_state_desc.SampleDesc, sizeof(DXGI_SAMPLE_DESC));
        graphics_hash = hash_combine(graphics_hash, &m_pipeline_state_desc.Flags, sizeof(D3D12_PIPELINE_STATE_FLAGS));
        for (uint32_t i = 0; i < m_pipeline_state_desc.InputLayout.NumElements; i++)
            graphics_hash = hash_combine(graphics_hash, &m_pipeline_state_desc.InputLayout.pInputElementDescs[i], sizeof(D3D12_INPUT_ELEMENT_DESC));

        swprintf_s(pipeline_name, L"%S_%zu_%zu_%zu", "GraphicsPSO", shader_hash, graphics_hash, reinterpret_cast<size_t>(dx12_root_signature->m_root_signature));
        result = dx12_device->m_pipeline_library->LoadGraphicsPipeline(pipeline_name, &m_pipeline_state_desc, IID_PPV_ARGS(&m_pipeline_state));
    }
    if (FAILED(result))
    {
        DX_CHECK_RESULT(dx12_device->m_device->CreateGraphicsPipelineState(&m_pipeline_state_desc, IID_PPV_ARGS(&m_pipeline_state)));
        if (dx12_device->m_pipeline_library)
        {
            if (SUCCEEDED(dx12_device->m_pipeline_library->StorePipeline(pipeline_name, m_pipeline_state)))
                RENDERING_LOG_INFO("store pso to pipeline library succeed!");
            else
                RENDERING_LOG_WARNING("store pso to pipeline library failed!");
        }
    }

    switch (info.primitive_topology)
    {
    case GPUPrimitiveTopology::e_point_list:
        m_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    case GPUPrimitiveTopology::e_line_list:
        m_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case GPUPrimitiveTopology::e_line_strip:
        m_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
    case GPUPrimitiveTopology::e_triangle_list:
        m_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case GPUPrimitiveTopology::e_triangle_strip:
        m_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    default:
        break;
    }

    m_root_signature = dx12_root_signature->m_root_signature;
}

DX12GraphicsPipeline::~DX12GraphicsPipeline()
{
    DX_FREE(m_pipeline_state);
}

AMAZING_NAMESPACE_END