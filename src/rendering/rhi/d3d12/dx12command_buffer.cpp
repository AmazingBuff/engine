//
// Created by AmazingBuff on 2025/4/17.
//

#include "dx12command_buffer.h"
#include "dx12device.h"
#include "dx12command_pool.h"
#include "dx12query_pool.h"
#include "dx12graphics_pass_encoder.h"
#include "resources/dx12buffer.h"
#include "resources/dx12texture.h"
#include "resources/dx12texture_view.h"
#include "utils/dx_macro.h"
#include "utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

static constexpr D3D12_COMMAND_LIST_TYPE Command_List_Map[GPU_Queue_Type_Count] =
{
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    D3D12_COMMAND_LIST_TYPE_COMPUTE,
    D3D12_COMMAND_LIST_TYPE_COPY
};


DX12CommandBuffer::DX12CommandBuffer(GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info) : m_command_list(nullptr), m_bound_descriptor_heaps{}, m_bound_heap_index(0), m_bound_root_signature(nullptr)
{
    DX12CommandPool const* dx12_command_pool = static_cast<DX12CommandPool const*>(pool);
    DX12Device const* dx12_device = static_cast<DX12Device const*>(dx12_command_pool->m_ref_device);

    m_bound_heap_index = GPU_Node_Index;

    ID3D12GraphicsCommandList* command = nullptr;
    DX_CHECK_RESULT(dx12_device->m_device->CreateCommandList(m_bound_heap_index,
        Command_List_Map[to_underlying(dx12_command_pool->m_type)], dx12_command_pool->m_command_allocator,
        nullptr, IID_PPV_ARGS(&command)));
    DX_CHECK_RESULT(command->Close());

    if (FAILED(command->QueryInterface(IID_PPV_ARGS(&m_command_list))))
        RENDERING_LOG_ERROR("can't support ID3D12GraphicsCommandList4!");

    DX_FREE(command);

    m_bound_descriptor_heaps[0] = dx12_device->m_descriptor_heap->m_gpu_cbv_srv_uav_heaps[m_bound_heap_index];
    m_bound_descriptor_heaps[1] = dx12_device->m_descriptor_heap->m_gpu_sampler_heaps[m_bound_heap_index];
    m_ref_pool = dx12_command_pool;
}

DX12CommandBuffer::~DX12CommandBuffer()
{
    DX_FREE(m_command_list);
}

void DX12CommandBuffer::begin_command()
{
    DX12CommandPool const* dx12_command_pool = static_cast<DX12CommandPool const*>(m_ref_pool);

    DX_CHECK_RESULT(m_command_list->Reset(dx12_command_pool->m_command_allocator, nullptr));

    if (dx12_command_pool->m_type != GPUQueueType::e_transfer)
    {
        ID3D12DescriptorHeap* heaps[] = {
            m_bound_descriptor_heaps[0]->descriptor_heap,
            m_bound_descriptor_heaps[1]->descriptor_heap
        };
        m_command_list->SetDescriptorHeaps(2, heaps);
    }

    m_bound_root_signature = nullptr;
}

void DX12CommandBuffer::end_command()
{
    DX_CHECK_RESULT(m_command_list->Close());
}

void DX12CommandBuffer::begin_query(GPUQueryPool const* pool, GPUQueryInfo const& info)
{
    DX12QueryPool const* dx12_query_pool = static_cast<DX12QueryPool const*>(pool);

    switch (dx12_query_pool->m_query_type)
    {
    case GPUQueryType::e_timestamp:
        break;
    default:
        m_command_list->BeginQuery(dx12_query_pool->m_query_heap, Query_Type_Map[to_underlying(dx12_query_pool->m_query_type)], info.index);
        break;
    }
}

void DX12CommandBuffer::end_query(GPUQueryPool const* pool, GPUQueryInfo const& info)
{
    DX12QueryPool const* dx12_query_pool = static_cast<DX12QueryPool const*>(pool);

    m_command_list->EndQuery(dx12_query_pool->m_query_heap, Query_Type_Map[to_underlying(dx12_query_pool->m_query_type)], info.index);
}

GPUGraphicsPassEncoder* DX12CommandBuffer::begin_graphics_pass(GPUGraphicsPassCreateInfo const& info)
{
    D3D12_RENDER_PASS_ENDING_ACCESS_RESOLVE_SUBRESOURCE_PARAMETERS sub_resolve_subresource[GPU_Max_Render_Target]{};
    D3D12_RENDER_PASS_RENDER_TARGET_DESC render_target_desc[GPU_Max_Render_Target]{};
    for (size_t i = 0; i < info.color_attachment_count; i++)
    {
        GPUColorAttachment const& color_attachment = info.color_attachments[i];
        DX12TextureView const* texture_view = static_cast<DX12TextureView const*>(color_attachment.texture_view);

        D3D12_CLEAR_VALUE color_clear_value{
            .Format = transfer_format(texture_view->m_format),
            .Color{color_attachment.clear_color.color.r, color_attachment.clear_color.color.g,
            color_attachment.clear_color.color.b, color_attachment.clear_color.color.a}
        };

        D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE render_pass_begin_access_type = Render_Pass_Begin_Map[to_underlying(color_attachment.load)];
        if (info.sample_count > GPUSampleCount::e_1 && color_attachment.resolve_view)
        {
            DX12TextureView const* resolve_view = static_cast<DX12TextureView const*>(color_attachment.resolve_view);
            DX12Texture const* texture = static_cast<DX12Texture const*>(texture_view->m_ref_texture);
            DX12Texture const* resolve = static_cast<DX12Texture const*>(resolve_view->m_ref_texture);

            sub_resolve_subresource[i].SrcRect = { 0, 0, static_cast<int>(texture->m_info->width), static_cast<int>(texture->m_info->height) };

            D3D12_RENDER_PASS_ENDING_ACCESS_TYPE render_pass_end_access_type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_RESOLVE;
            D3D12_RENDER_PASS_ENDING_ACCESS_RESOLVE_PARAMETERS resolve_parameters{
                .pSrcResource = texture->m_resource,
                .pDstResource = resolve->m_resource,
                .SubresourceCount = 1,
                .pSubresourceParameters = &sub_resolve_subresource[i],
                .Format = color_clear_value.Format,
                .ResolveMode = D3D12_RESOLVE_MODE_AVERAGE,
                .PreserveResolveSource = false,
            };

            render_target_desc[i].EndingAccess = { render_pass_end_access_type, {resolve_parameters} };
        }
        else
        {
            D3D12_RENDER_PASS_ENDING_ACCESS_TYPE render_pass_end_access_type = Render_Pass_End_Map[to_underlying(color_attachment.store)];
            render_target_desc[i].EndingAccess = { render_pass_end_access_type, {} };
        }
        render_target_desc[i].cpuDescriptor = texture_view->m_rtv_dsv_handle;
        render_target_desc[i].BeginningAccess = { render_pass_begin_access_type, {color_clear_value} };
    }

    // depth stencil
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* depth_stencil_desc = nullptr;
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depth_stencil{};
    if (info.depth_stencil_attachment && info.depth_stencil_attachment->texture_view)
    {
        DX12TextureView const* texture_view = static_cast<DX12TextureView const*>(info.depth_stencil_attachment->texture_view);
        D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE depth_begin_access = Render_Pass_Begin_Map[to_underlying(info.depth_stencil_attachment->depth_load)];
        D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE stencil_begin_access = Render_Pass_Begin_Map[to_underlying(info.depth_stencil_attachment->stencil_load)];
        D3D12_RENDER_PASS_ENDING_ACCESS_TYPE depth_end_access = Render_Pass_End_Map[to_underlying(info.depth_stencil_attachment->depth_store)];
        D3D12_RENDER_PASS_ENDING_ACCESS_TYPE stencil_end_access = Render_Pass_End_Map[to_underlying(info.depth_stencil_attachment->stencil_store)];

        D3D12_CLEAR_VALUE depth_clear_value{
            .Format = transfer_format(texture_view->m_format),
            .DepthStencil{
                .Depth = info.depth_stencil_attachment->clear_color.depth_stencil.depth,
            },
        };
        D3D12_CLEAR_VALUE stencil_clear_value{
            .Format = transfer_format(texture_view->m_format),
            .DepthStencil{
                .Stencil = static_cast<uint8_t>(info.depth_stencil_attachment->clear_color.depth_stencil.stencil),
            },
        };

        depth_stencil.cpuDescriptor = texture_view->m_rtv_dsv_handle;
        depth_stencil.DepthBeginningAccess = { depth_begin_access, {depth_clear_value} };
        depth_stencil.DepthEndingAccess = { depth_end_access, {} };
        depth_stencil.StencilBeginningAccess = { stencil_begin_access, {stencil_clear_value} };
        depth_stencil.StencilEndingAccess = { stencil_end_access, {} };
        depth_stencil_desc = &depth_stencil;
    }

    m_command_list->BeginRenderPass(info.color_attachment_count, render_target_desc, depth_stencil_desc, D3D12_RENDER_PASS_FLAG_NONE);

    DX12GraphicsPassEncoder* encoder = PLACEMENT_NEW(DX12GraphicsPassEncoder, sizeof(DX12GraphicsPassEncoder));
    encoder->m_command_buffer = this;
    return encoder;
}

void DX12CommandBuffer::end_graphics_pass(GPUGraphicsPassEncoder* encoder)
{
    PLACEMENT_DELETE(DX12GraphicsPassEncoder, static_cast<DX12GraphicsPassEncoder*>(encoder));
    m_command_list->EndRenderPass();
}

void DX12CommandBuffer::transfer_buffer_to_texture(GPUBufferToTextureTransferInfo const& info)
{
    DX12CommandPool const* command_pool = static_cast<DX12CommandPool const*>(m_ref_pool);
    DX12Device const* device = static_cast<DX12Device const*>(command_pool->m_ref_device);
    DX12Buffer const* buffer = static_cast<DX12Buffer const*>(info.src_buffer);
    DX12Texture const* texture = static_cast<DX12Texture const*>(info.dst_texture);

    GPUTextureSubresource const& subresource = info.dst_texture_subresource;
    uint32_t subresource_index = transfer_subresource_index(subresource.mip_level, subresource.base_array_layer,
        0, 1, subresource.array_layers);

    D3D12_RESOURCE_DESC texture_desc = texture->m_resource->GetDesc();
    D3D12_TEXTURE_COPY_LOCATION src_copy{
        .pResource = buffer->m_resource,
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
    };
    device->m_device->GetCopyableFootprints(&texture_desc, subresource_index, 1, info.src_buffer_offset, &src_copy.PlacedFootprint, nullptr, nullptr, nullptr);

    src_copy.PlacedFootprint.Offset = info.src_buffer_offset;
    D3D12_TEXTURE_COPY_LOCATION dst_copy{
        .pResource = texture->m_resource,
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = subresource_index
    };
    m_command_list->CopyTextureRegion(&dst_copy, 0, 0, 0, &src_copy, nullptr);
}

void DX12CommandBuffer::resource_barrier(GPUResourceBarrierInfo const& info)
{
    D3D12_RESOURCE_BARRIER* resource_barriers = static_cast<D3D12_RESOURCE_BARRIER*>(alloca(
        (info.buffer_barriers.size() + info.texture_barriers.size()) * sizeof(D3D12_RESOURCE_BARRIER)));

    uint32_t transition_count = 0;
    for (const GPUBufferBarrier& barrier : info.buffer_barriers)
    {
        D3D12_RESOURCE_BARRIER& resource = resource_barriers[transition_count];
        DX12Buffer const* buffer = static_cast<DX12Buffer const*>(barrier.buffer);
        GPUMemoryUsage memory_usage = buffer->m_info->memory_usage;
        GPUResourceType resource_type = buffer->m_info->type;
        if (memory_usage == GPUMemoryUsage::e_gpu_only || memory_usage == GPUMemoryUsage::e_gpu_to_cpu ||
            (memory_usage == GPUMemoryUsage::e_cpu_to_gpu && (resource_type & GPUResourceTypeFlag::e_rw_buffer)))
        {
            if (barrier.src_state == GPUResourceStateFlag::e_unordered_access &&
                barrier.dst_state == GPUResourceStateFlag::e_unordered_access)
            {
                resource.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
                resource.UAV.pResource = buffer->m_resource;
                resource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            }
            else
            {
                resource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                resource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                resource.Transition.pResource = buffer->m_resource;
                resource.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

                if (barrier.queue_acquire)
                    resource.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
                else
                    resource.Transition.StateBefore = transfer_resource_state(barrier.src_state);

                if (barrier.queue_release)
                    resource.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
                else
                    resource.Transition.StateAfter = transfer_resource_state(barrier.dst_state);
            }
            transition_count++;
        }
    }
    for (const GPUTextureBarrier& barrier : info.texture_barriers)
    {
        D3D12_RESOURCE_BARRIER& resource = resource_barriers[transition_count];
        DX12Texture const* texture = static_cast<DX12Texture const*>(barrier.texture);
        if (barrier.src_state == GPUResourceStateFlag::e_unordered_access &&
            barrier.dst_state == GPUResourceStateFlag::e_unordered_access)
        {
            resource.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            resource.UAV.pResource = texture->m_resource;
            resource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        }
        else
        {
            resource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            resource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            resource.Transition.pResource = texture->m_resource;
            resource.Transition.Subresource = barrier.subresource_barrier ?
                transfer_subresource_index(barrier.mip_level, barrier.array_layer, 0, texture->m_info->mip_levels, texture->m_info->array_layers) :
                D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            if (barrier.queue_acquire)
                resource.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
            else
                resource.Transition.StateBefore = transfer_resource_state(barrier.src_state);

            if (barrier.queue_release)
                resource.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
            else
                resource.Transition.StateAfter = transfer_resource_state(barrier.dst_state);

            if (resource.Transition.StateBefore == D3D12_RESOURCE_STATE_COMMON &&
                resource.Transition.StateAfter == D3D12_RESOURCE_STATE_COMMON &&
                (barrier.src_state == GPUResourceStateFlag::e_present || barrier.dst_state == GPUResourceStateFlag::e_present))
                continue;
        }
        transition_count++;
    }
    if (transition_count)
        m_command_list->ResourceBarrier(transition_count, resource_barriers);
}

void DX12CommandBuffer::reset_root_signature(GPUPipelineType type, ID3D12RootSignature* root_signature)
{
    if (m_bound_root_signature != root_signature)
    {
        m_bound_root_signature = root_signature;
        if (type == GPUPipelineType::e_graphics)
            m_command_list->SetGraphicsRootSignature(root_signature);
        else
            m_command_list->SetComputeRootSignature(root_signature);
    }
}


AMAZING_NAMESPACE_END
