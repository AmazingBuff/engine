//
// Created by AmazingBuff on 2025/4/27.
//

#include "dx12graphics_pass_encoder.h"
#include "dx12descriptor_set.h"
#include "dx12command_buffer.h"
#include "dx12root_signature.h"
#include "dx12graphics_pipeline.h"
#include "resources/dx12buffer.h"

AMAZING_NAMESPACE_BEGIN

DX12GraphicsPassEncoder::DX12GraphicsPassEncoder() : m_command_buffer(nullptr) {}

void DX12GraphicsPassEncoder::bind_vertex_buffers(GPUBufferBinding const* bindings, uint32_t binding_count)
{
    Vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_views(binding_count);
    for (size_t i = 0; i < binding_count; i++)
    {
        GPUBufferBinding const& binding = bindings[i];
        DX12Buffer const* buffer = static_cast<DX12Buffer const*>(binding.buffer);

        vertex_buffer_views[i].BufferLocation = buffer->m_gpu_address + binding.offset;
        vertex_buffer_views[i].StrideInBytes = binding.stride;
        vertex_buffer_views[i].SizeInBytes = buffer->m_info->size - binding.offset;
    }

    m_command_buffer->m_command_list->IASetVertexBuffers(0, binding_count, vertex_buffer_views.data());
}

void DX12GraphicsPassEncoder::bind_index_buffer(GPUBufferBinding const& binding)
{
    DX12Buffer const* buffer = static_cast<DX12Buffer const*>(binding.buffer);
    D3D12_INDEX_BUFFER_VIEW index_buffer_view{
        .BufferLocation = buffer->m_gpu_address + binding.offset,
        .SizeInBytes = static_cast<uint32_t>(buffer->m_info->size - binding.offset),
        .Format = DXGI_FORMAT_R32_UINT,
    };
    m_command_buffer->m_command_list->IASetIndexBuffer(&index_buffer_view);
}

void DX12GraphicsPassEncoder::bind_descriptor_set(GPUDescriptorSet const* set)
{
    DX12DescriptorSet const* descriptor_set = static_cast<DX12DescriptorSet const*>(set);
    DX12RootSignature const* root_signature = static_cast<DX12RootSignature const*>(descriptor_set->m_ref_root_signature);

    m_command_buffer->reset_root_signature(GPUPipelineType::e_graphics, root_signature->m_root_signature);
    if (descriptor_set->m_cbv_srv_uav_handle != 0)
        m_command_buffer->m_command_list->SetGraphicsRootDescriptorTable(descriptor_set->m_set_index,
            { m_command_buffer->m_bound_descriptor_heaps[0]->start_handle.gpu.ptr + descriptor_set->m_cbv_srv_uav_handle });
    else
        m_command_buffer->m_command_list->SetGraphicsRootDescriptorTable(descriptor_set->m_set_index,
            { m_command_buffer->m_bound_descriptor_heaps[1]->start_handle.gpu.ptr + descriptor_set->m_sampler_handle });
}

void DX12GraphicsPassEncoder::bind_pipeline(GPUGraphicsPipeline const* pipeline)
{
    DX12GraphicsPipeline const* dx12_pipeline = static_cast<DX12GraphicsPipeline const*>(pipeline);
    DX12RootSignature const* dx12_root_signature = static_cast<DX12RootSignature const*>(dx12_pipeline->m_ref_root_signature);

    m_command_buffer->reset_root_signature(GPUPipelineType::e_graphics, dx12_root_signature->m_root_signature);
    m_command_buffer->m_command_list->IASetPrimitiveTopology(dx12_pipeline->m_primitive_topology);
    m_command_buffer->m_command_list->SetPipelineState(dx12_pipeline->m_pipeline_state);
}

void DX12GraphicsPassEncoder::set_viewport(float x, float y, float width, float height, float min_depth, float max_depth)
{
    D3D12_VIEWPORT viewport{
        .TopLeftX = x,
        .TopLeftY = y,
        .Width = width,
        .Height = height,
        .MinDepth = min_depth,
        .MaxDepth = max_depth,
    };
    m_command_buffer->m_command_list->RSSetViewports(1, &viewport);
}

void DX12GraphicsPassEncoder::set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    D3D12_RECT scissor{
        .left = static_cast<int>(x),
        .top = static_cast<int>(y),
        .right = static_cast<int>(x + width),
        .bottom = static_cast<int>(y + height),
    };
    m_command_buffer->m_command_list->RSSetScissorRects(1, &scissor);
}

void DX12GraphicsPassEncoder::set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data)
{
    DX12RootSignature const* dx12_root_signature = static_cast<DX12RootSignature const*>(root_signature);

    m_command_buffer->reset_root_signature(GPUPipelineType::e_graphics, dx12_root_signature->m_root_signature);

    for (uint32_t i = 0; i < dx12_root_signature->m_push_constant_count; i++)
    {
        if (name == dx12_root_signature->m_push_constants[i].name)
        {
            m_command_buffer->m_command_list->SetGraphicsRoot32BitConstants(dx12_root_signature->m_constant_parameters[i].index,
                dx12_root_signature->m_constant_parameters[i].root_parameter.Constants.Num32BitValues, data, 0);
        }
    }
}

void DX12GraphicsPassEncoder::draw(uint32_t vertex_count, uint32_t first_vertex)
{
    m_command_buffer->m_command_list->DrawInstanced(vertex_count, 1, first_vertex, 0);
}

void DX12GraphicsPassEncoder::draw_instance(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    m_command_buffer->m_command_list->DrawInstanced(vertex_count, instance_count, first_vertex, first_instance);
}

void DX12GraphicsPassEncoder::draw_indexed(uint32_t index_count, uint32_t first_index, uint32_t first_vertex)
{
    m_command_buffer->m_command_list->DrawIndexedInstanced(index_count, 1, first_index, static_cast<int>(first_vertex), 0);
}

void DX12GraphicsPassEncoder::draw_indexed_instance(uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t first_vertex, uint32_t first_instance)
{
    m_command_buffer->m_command_list->DrawIndexedInstanced(index_count, instance_count, first_index, static_cast<int>(first_vertex), first_instance);
}



AMAZING_NAMESPACE_END
