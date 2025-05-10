//
// Created by AmazingBuff on 25-5-10.
//

#pragma once

#include "vk.h"
#include "rendering/rhi/common/swapchian.h"

AMAZING_NAMESPACE_BEGIN

class VKSwapChain final : public GPUSwapChain
{
public:
    VKSwapChain(GPUDevice const* device, GPUSwapChainCreateInfo const& info);
    ~VKSwapChain() override;

    uint32_t acquire_next_frame(GPUSemaphore const* semaphore, GPUFence const* fence) override;
    GPUTexture const* fetch_back_texture(uint32_t index) const override;
    GPUTextureView const* fetch_back_texture_view(uint32_t index) const override;

private:
    VkSwapchainKHR m_swap_chain;

    friend class VKQueue;
};

AMAZING_NAMESPACE_END