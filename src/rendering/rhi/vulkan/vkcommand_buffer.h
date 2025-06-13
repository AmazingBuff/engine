//
// Created by AmazingBuff on 25-5-10.
//

#pragma once

#include "vk.h"
#include "rendering/rhi/common/command_buffer.h"

AMAZING_NAMESPACE_BEGIN

class VKTexture;

class VKCommandBuffer final : public GPUCommandBuffer
{
public:
    VKCommandBuffer(GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info);
    ~VKCommandBuffer() override;

    void begin_command() override;
    void end_command() override;

    void begin_query(GPUQueryPool const* pool, GPUQueryInfo const& info) override;
    void end_query(GPUQueryPool const* pool, GPUQueryInfo const& info) override;
    GPUGraphicsPassEncoder* begin_graphics_pass(GPUGraphicsPassCreateInfo const& info) override;
    void end_graphics_pass(GPUGraphicsPassEncoder* encoder) override;
    GPUComputePassEncoder* begin_compute_pass(GPUComputePassCreateInfo const& info) override;
    void end_compute_pass(GPUComputePassEncoder* encoder) override;

    void transfer_buffer_to_texture(GPUBufferToTextureTransferInfo const& info) override;
    void resource_barrier(GPUResourceBarrierInfo const& info) override;
    void generate_mipmap(GPUTexture const* texture, const GPUResourceState& src_state, const GPUResourceState& dst_state) override;
private:
    VkCommandBuffer m_command_buffer;

    friend class VKQueue;
    friend class VKGraphicsPassEncoder;
    friend class VKComputePassEncoder;
};

AMAZING_NAMESPACE_END