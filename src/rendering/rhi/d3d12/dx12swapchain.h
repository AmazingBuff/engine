//
// Created by Administrator on 2025/4/11.
//

#ifndef DX12SWAPCHAIN_H
#define DX12SWAPCHAIN_H

#include "resources/dx12texture.h"
#include "rendering/rhi/common/swapchian.h"

AMAZING_NAMESPACE_BEGIN

class DX12SwapChain final : public GPUSwapChain
{
public:
    DX12SwapChain();
    ~DX12SwapChain() override;

    AResult initialize(GPUInstance const* instance, GPUDevice const* device, GPUSwapChainCreateInfo const& info) override;
    uint32_t acquire_next_frame(GPUSemaphore const* semaphore, GPUFence const* fence) override;
private:
    struct D3D12SwapChainBuffer
    {
        ID3D12Resource* resource;
        DX12Texture::GPUTextureInfo tex_info;
    };

    IDXGISwapChain3* m_swap_chain;
    Vector<D3D12SwapChainBuffer> m_swap_chain_buffer;
};


AMAZING_NAMESPACE_END


#endif //DX12SWAPCHAIN_H
