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
    DX12SwapChain(GPUDevice const* device, GPUSwapChainCreateInfo const& info);
    ~DX12SwapChain() override;

    uint32_t acquire_next_frame(GPUSemaphore const* semaphore, GPUFence* fence) override;
    GPUTexture const* fetch_back_texture(uint32_t index) const override;
    GPUTextureView const* fetch_back_texture_view(uint32_t index) const override;
private:
    struct D3D12SwapChainBuffer
    {
        ID3D12Resource* resource;
        DX12Texture::GPUTextureInfo tex_info;
    };

    IDXGISwapChain3* m_swap_chain;
    //Vector<D3D12SwapChainBuffer> m_swap_chain_buffer;
    uint32_t m_present_sync_interval;
    uint32_t m_present_flags;

    friend class DX12Queue;
};


AMAZING_NAMESPACE_END


#endif //DX12SWAPCHAIN_H
