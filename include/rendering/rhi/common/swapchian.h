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
    GPUSwapChain() : m_ref_device(nullptr) {}
    virtual ~GPUSwapChain() = default;

    virtual uint32_t acquire_next_frame(GPUSemaphore const* semaphore, GPUFence const* fence) = 0;
    virtual GPUTexture const* fetch_back_texture(uint32_t index) const = 0;
    virtual GPUTextureView const* fetch_back_texture_view(uint32_t index) const = 0;
protected:
    GPUDevice const* m_ref_device;
    struct GPUSwapChainBackBuffer
    {
        GPUTexture* back_texture;
        GPUTextureView* back_texture_view;
    };

    Vector<GPUSwapChainBackBuffer> m_back_textures;
};

AMAZING_NAMESPACE_END


#endif //SWAPCHIAN_H
