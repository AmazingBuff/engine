//
// Created by AmazingBuff on 2025/5/26.
//

#include "dx12compute_pass_encoder.h"
#include "dx12compute_pipeline.h"
#include "dx12root_signature.h"
#include "dx12command_buffer.h"
#include "dx12descriptor_set.h"

AMAZING_NAMESPACE_BEGIN

DX12ComputePassEncoder::DX12ComputePassEncoder() : m_command_buffer(nullptr) {}

void DX12ComputePassEncoder::bind_descriptor_set(GPUDescriptorSet const* set)
{
    DX12DescriptorSet const* descriptor_set = static_cast<DX12DescriptorSet const*>(set);
    m_command_buffer->m_command_list->SetComputeRootDescriptorTable(descriptor_set->m_set_index, {m_command_buffer->m_bound_descriptor_heaps[0]->start_handle.gpu.ptr + descriptor_set->m_cbv_srv_uav_handle});
}

void DX12ComputePassEncoder::bind_pipeline(GPUComputePipeline const* pipeline)
{
    DX12ComputePipeline const* dx12_compute_pipeline = static_cast<DX12ComputePipeline const*>(pipeline);
    DX12RootSignature const* dx12_root_signature = static_cast<DX12RootSignature const*>(dx12_compute_pipeline->m_ref_root_signature);

    m_command_buffer->reset_root_signature(GPUPipelineType::e_compute, dx12_root_signature->m_root_signature);
    m_command_buffer->m_command_list->SetPipelineState(dx12_compute_pipeline->m_pipeline_state);
}

void DX12ComputePassEncoder::set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data)
{
    DX12RootSignature const* dx12_root_signature = static_cast<DX12RootSignature const*>(root_signature);
    m_command_buffer->reset_root_signature(GPUPipelineType::e_compute, dx12_root_signature->m_root_signature);
    for (uint32_t i = 0; i < dx12_root_signature->m_push_constant_count; i++)
    {
        if (name == dx12_root_signature->m_push_constants[i].name)
        {
            m_command_buffer->m_command_list->SetComputeRoot32BitConstants(dx12_root_signature->m_constant_parameters[i].index,
                dx12_root_signature->m_constant_parameters[i].root_parameter.Constants.Num32BitValues, data, 0);
        }
    }
}

void DX12ComputePassEncoder::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    m_command_buffer->m_command_list->Dispatch(x, y, z);
}

AMAZING_NAMESPACE_END