//
// Created by AmazingBuffer on 2025/4/11.
//

#ifndef SWAPCHIAN_H
#define SWAPCHIAN_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUSwapChain
{
public:
    GPUSwapChain() = default;
    virtual ~GPUSwapChain() = default;

    virtual AResult initialize(GPUInstance const* instance, GPUDevice const* device, GPUSwapChainCreateInfo const& info) = 0;
    virtual uint32_t acquire_next_frame(GPUSemaphore const* semaphore, GPUFence const* fence) = 0;
};

AMAZING_NAMESPACE_END


#endif //SWAPCHIAN_H
