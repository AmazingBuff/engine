//
// Created by AmazingBuff on 2025/5/26.
//

#include "dx12compute_pipeline.h"
#include "dx12root_signature.h"
#include "dx12device.h"
#include "resources/dx12shader_library.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12ComputePipeline::DX12ComputePipeline(GPUComputePipelineCreateInfo const& info)
{
    DX12RootSignature const* dx12_root_signature = static_cast<DX12RootSignature const*>(info.root_signature);
    DX12Device const* dx12_device = static_cast<DX12Device const*>(dx12_root_signature->m_ref_device);

    DX12ShaderLibrary const* dx12_shader_library = static_cast<DX12ShaderLibrary const*>(info.compute_shader->library);
    D3D12_SHADER_BYTECODE compute_shader{
        .pShaderBytecode = dx12_shader_library->m_blob_encoding->GetBufferPointer(),
        .BytecodeLength = dx12_shader_library->m_blob_encoding->GetBufferSize()
    };

    D3D12_CACHED_PIPELINE_STATE cached_pipeline_state{
        .pCachedBlob = nullptr,
        .CachedBlobSizeInBytes = 0,
    };

    D3D12_COMPUTE_PIPELINE_STATE_DESC pipeline_state_desc{
        .pRootSignature = dx12_root_signature->m_root_signature,
        .CS = compute_shader,
        .NodeMask = GPU_Node_Mask,
        .CachedPSO = cached_pipeline_state,
        .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
    };

    // create
    HRESULT result = E_FAIL;
    wchar_t pipeline_name[DX12_Pipeline_State_Object_Name_Max_Length]{};
    uint64_t shader_hash = Rendering_Hash;
    uint64_t graphics_hash = Rendering_Hash;
    if (dx12_device->m_pipeline_library)
    {
        if (compute_shader.pShaderBytecode)
            shader_hash = hash_combine(shader_hash, compute_shader.pShaderBytecode, compute_shader.BytecodeLength);

        graphics_hash = hash_combine(graphics_hash, &pipeline_state_desc.NodeMask, sizeof(uint32_t));
        graphics_hash = hash_combine(graphics_hash, &pipeline_state_desc.Flags, sizeof(D3D12_PIPELINE_STATE_FLAGS));
        swprintf_s(pipeline_name, L"%S_%zu_%zu_%zu", "ComputePSO", shader_hash, graphics_hash, reinterpret_cast<size_t>(dx12_root_signature->m_root_signature));
        result = dx12_device->m_pipeline_library->LoadComputePipeline(pipeline_name, &pipeline_state_desc, IID_PPV_ARGS(&m_pipeline_state));
    }
    if (FAILED(result))
    {
        DX_CHECK_RESULT(dx12_device->m_device->CreateComputePipelineState(&pipeline_state_desc, IID_PPV_ARGS(&m_pipeline_state)));
        if (dx12_device->m_pipeline_library)
        {
            if (SUCCEEDED(dx12_device->m_pipeline_library->StorePipeline(pipeline_name, m_pipeline_state)))
                RENDERING_LOG_INFO("store pso to pipeline library succeed!");
            else
                RENDERING_LOG_WARNING("store pso to pipeline library failed!");
        }
    }

    m_ref_root_signature = info.root_signature;
}

DX12ComputePipeline::~DX12ComputePipeline()
{
    DX_FREE(m_pipeline_state);
}


AMAZING_NAMESPACE_END
