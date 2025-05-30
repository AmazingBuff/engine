//
// Created by AmazingBuff on 25-5-10.
//

#pragma once

#include "vk.h"
#include "rendering/rhi/common/command_pool.h"

AMAZING_NAMESPACE_BEGIN

class VKCommandPool final : public GPUCommandPool
{
public:
    VKCommandPool(GPUQueue const* queue, GPUCommandPoolCreateInfo const& info);
    ~VKCommandPool() override;

    void reset() override;
private:
    VkCommandPool m_pool;

    friend class VKCommandBuffer;
    friend class VKGraphicsPassEncoder;
    friend class VKComputePassEncoder;
    friend class VKTexture;
};

AMAZING_NAMESPACE_END