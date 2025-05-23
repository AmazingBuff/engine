//
// Created by AmazingBuff on 2025/4/23.
//

#include <windows.h>
#include <dxc/d3d12shader.h>

#include "dx12shader_library.h"
#include "rendering/rhi/d3d12/dx12.h"
#include "rendering/rhi/d3d12/dx12device.h"
#include "rendering/rhi/d3d12/utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

static constexpr GPUResourceTypeFlag Resource_Type_Flag_Map[] =
{
    GPUResourceTypeFlag::e_uniform_buffer,     // D3D_SIT_CBUFFER
    GPUResourceTypeFlag::e_buffer,             // D3D_SIT_TBUFFER
    GPUResourceTypeFlag::e_texture,            // D3D_SIT_TEXTURE
    GPUResourceTypeFlag::e_sampler,            // D3D_SIT_SAMPLER
    GPUResourceTypeFlag::e_rw_texture,         // D3D_SIT_UAV_RWTYPED
    GPUResourceTypeFlag::e_buffer,             // D3D_SIT_STRUCTURED
    GPUResourceTypeFlag::e_rw_buffer,          // D3D_SIT_RWSTRUCTURED
    GPUResourceTypeFlag::e_buffer,             // D3D_SIT_BYTEADDRESS
    GPUResourceTypeFlag::e_rw_buffer,          // D3D_SIT_UAV_RWBYTEADDRESS
    GPUResourceTypeFlag::e_rw_buffer,          // D3D_SIT_UAV_APPEND_STRUCTURED
    GPUResourceTypeFlag::e_rw_buffer,          // D3D_SIT_UAV_CONSUME_STRUCTURED
    GPUResourceTypeFlag::e_rw_buffer,          // D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER
    GPUResourceTypeFlag::e_ray_tracing,        // D3D_SIT_RTACCELERATIONSTRUCTURE
};

static constexpr GPUTextureType Texture_Type_Map[] =
{
    GPUTextureType::e_undefined,      // D3D_SRV_DIMENSION_UNKNOWN
    GPUTextureType::e_undefined,      // D3D_SRV_DIMENSION_BUFFER
    GPUTextureType::e_1d,             // D3D_SRV_DIMENSION_TEXTURE1D
    GPUTextureType::e_1d_array,       // D3D_SRV_DIMENSION_TEXTURE1DARRAY
    GPUTextureType::e_2d,             // D3D_SRV_DIMENSION_TEXTURE2D
    GPUTextureType::e_2d_array,       // D3D_SRV_DIMENSION_TEXTURE2DARRAY
    GPUTextureType::e_2dms,           // D3D_SRV_DIMENSION_TEXTURE2DMS
    GPUTextureType::e_2dms_array,     // D3D_SRV_DIMENSION_TEXTURE2DMSARRAY
    GPUTextureType::e_3d,             // D3D_SRV_DIMENSION_TEXTURE3D
    GPUTextureType::e_cube,           // D3D_SRV_DIMENSION_TEXTURECUBE
    GPUTextureType::e_cube_array,     // D3D_SRV_DIMENSION_TEXTURECUBEARRAY
    GPUTextureType::e_undefined       // D3D_SRV_DIMENSION_BUFFEREX
};

static constexpr GPUFormat Format_Map[] =
{
    GPUFormat::e_undefined,
    GPUFormat::e_r32_uint,
    GPUFormat::e_r32_sint,
    GPUFormat::e_r32_sfloat,

    GPUFormat::e_r32g32_uint,
    GPUFormat::e_r32g32_sint,
    GPUFormat::e_r32g32_sfloat,

    GPUFormat::e_r32g32b32_uint,
    GPUFormat::e_r32g32b32_sint,
    GPUFormat::e_r32g32b32_sfloat,

    GPUFormat::e_r32g32b32a32_uint,
    GPUFormat::e_r32g32b32a32_sint,
    GPUFormat::e_r32g32b32a32_sfloat,
};

DX12ShaderLibrary::DX12ShaderLibrary(GPUDevice const* device, GPUShaderLibraryCreateInfo const& info) : m_blob_encoding(nullptr)
{
    IDxcUtils* dxc_utils;
    DX_CHECK_RESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxc_utils)));
    DX_CHECK_RESULT(dxc_utils->CreateBlob(info.code, info.code_size, DXC_CP_ACP, &m_blob_encoding));

    // shader reflection
    IDxcContainerReflection* dxc_reflection;
    DX_CHECK_RESULT(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&dxc_reflection)));
    DX_CHECK_RESULT(dxc_reflection->Load(m_blob_encoding));

    uint32_t index;
    DX_CHECK_RESULT(dxc_reflection->FindFirstPartKind(DXC_PART_DXIL, &index));

    ID3D12ShaderReflection* dx12_reflection;
    DX_CHECK_RESULT(dxc_reflection->GetPartReflection(index, IID_PPV_ARGS(&dx12_reflection)));

    D3D12_SHADER_DESC shader_desc;
    DX_CHECK_RESULT(dx12_reflection->GetDesc(&shader_desc));

    // resources
    m_shader_reflections.resize(1);
    GPUShaderReflection& shader_reflection = m_shader_reflections[0];
    //shader_reflection.entry_name = info.name;
    shader_reflection.stage = info.stage;

    shader_reflection.shader_resources.resize(shader_desc.BoundResources);
    for (uint32_t i = 0; i < shader_desc.BoundResources; i++)
    {
        D3D12_SHADER_INPUT_BIND_DESC input_bind_desc;
        DX_CHECK_RESULT(dx12_reflection->GetResourceBindingDesc(i, &input_bind_desc));

        GPUShaderResource& shader_resource = shader_reflection.shader_resources[i];
        shader_resource.name = input_bind_desc.Name;
        shader_resource.name_hash = hash_str(input_bind_desc.Name, strlen(input_bind_desc.Name), DX12_Hash);
        shader_resource.resource_type = Resource_Type_Flag_Map[input_bind_desc.Type];
        shader_resource.set = input_bind_desc.Space;
        shader_resource.binding = input_bind_desc.BindPoint;
        shader_resource.array_count = input_bind_desc.BindCount;
        shader_resource.stage = info.stage;
        shader_resource.texture_type = Texture_Type_Map[input_bind_desc.Dimension];

        if (shader_desc.ConstantBuffers && input_bind_desc.Type == D3D_SIT_CBUFFER)
        {
            ID3D12ShaderReflectionConstantBuffer* constant_buffer = dx12_reflection->GetConstantBufferByName(input_bind_desc.Name);
            D3D12_SHADER_BUFFER_DESC buffer_desc;
            DX_CHECK_RESULT(constant_buffer->GetDesc(&buffer_desc));
            shader_resource.size = buffer_desc.Size;
        }
        if (input_bind_desc.Type == D3D_SIT_UAV_RWTYPED && input_bind_desc.Dimension == D3D_SRV_DIMENSION_BUFFER)
            shader_resource.resource_type = GPUResourceTypeFlag::e_rw_buffer;
        if (input_bind_desc.Type == D3D_SIT_TEXTURE && input_bind_desc.Dimension == D3D_SRV_DIMENSION_BUFFER)
            shader_resource.resource_type = GPUResourceTypeFlag::e_buffer;
    }

    // vertex inputs
    if (info.stage == GPUShaderStageFlag::e_vertex)
    {
        shader_reflection.vertex_inputs.resize(shader_desc.InputParameters);
        for (uint32_t i = 0; i < shader_desc.InputParameters; i++)
        {
            D3D12_SIGNATURE_PARAMETER_DESC input_param_desc;
            DX_CHECK_RESULT(dx12_reflection->GetInputParameterDesc(i, &input_param_desc));

            GPUVertexInput& vertex_input = shader_reflection.vertex_inputs[i];
            if (input_param_desc.SemanticIndex > 0 || strcmp(input_param_desc.SemanticName, "TEXCOORD") == 0)
                vertex_input.name = input_param_desc.SemanticName + to_str(input_param_desc.SemanticIndex);
            else
                vertex_input.name = input_param_desc.SemanticName;

            uint32_t channels = count_bits(input_param_desc.Mask);
            vertex_input.format = Format_Map[input_param_desc.ComponentType + 3 * (channels - 1)];
        }
    }
    else if (info.stage == GPUShaderStageFlag::e_compute)
    {
        dx12_reflection->GetThreadGroupSize(&shader_reflection.thread_group_sizes[0],
            &shader_reflection.thread_group_sizes[1], &shader_reflection.thread_group_sizes[2]);
    }

    dx12_reflection->Release();
    dxc_reflection->Release();
    dxc_utils->Release();

    m_ref_device = device;
}

DX12ShaderLibrary::~DX12ShaderLibrary()
{
    DX_FREE(m_blob_encoding);
}

AMAZING_NAMESPACE_END
