//
// Created by AmazingBuff on 2025/4/11.
//

#include "dx12swapchain.h"
#include "dx12instance.h"
#include "dx12adapter.h"
#include "dx12device.h"
#include "dx12queue.h"
#include "resources/dx12texture_view.h"
#include "utils/dx_macro.h"
#include "utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DX12SwapChain::DX12SwapChain(GPUDevice const* device, GPUSwapChainCreateInfo const& info) : m_swap_chain(nullptr), m_present_sync_interval(0), m_present_flags(0)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    DX12Adapter const* dx12_adapter = static_cast<DX12Adapter const*>(dx12_device->m_ref_adapter);
    DX12Instance const* dx12_instance = static_cast<DX12Instance const*>(dx12_adapter->m_ref_instance);

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{
        .Width = info.width,
        .Height = info.height,
        .Format = transfer_format(info.format),
        .Stereo = FALSE,
        .SampleDesc = {
            .Count = 1,
            .Quality = 0,
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = info.frame_count,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = 0,
    };

    BOOL allow_tearing = FALSE;
    if (SUCCEEDED(dx12_instance->m_dxgi_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing))) && allow_tearing)
    {
        swap_chain_desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        m_present_flags |= !info.enable_vsync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    }

    IDXGISwapChain1* swap_chain;
    HWND hwnd = reinterpret_cast<HWND>(info.surface);

    DX12Queue const* queue = nullptr;
    if (info.present_queues.empty())
        queue = static_cast<DX12Queue const*>(dx12_device->fetch_queue(GPUQueueType::e_graphics, 0));
    else
        queue = static_cast<DX12Queue const*>(info.present_queues[0]);

    DX_CHECK_RESULT(dx12_instance->m_dxgi_factory->CreateSwapChainForHwnd(queue->m_queue, hwnd, &swap_chain_desc, nullptr, nullptr, &swap_chain));
    DX_CHECK_RESULT(dx12_instance->m_dxgi_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
    DX_CHECK_RESULT(swap_chain->QueryInterface(IID_PPV_ARGS(&m_swap_chain)));

    DX_FREE(swap_chain);

    ID3D12Resource** buffers = static_cast<ID3D12Resource**>(alloca(info.frame_count * sizeof(ID3D12Resource*)));
    for (uint32_t i = 0; i < info.frame_count; i++)
        DX_CHECK_RESULT(m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&buffers[i])));

    m_back_textures.resize(info.frame_count);
    for (uint32_t i = 0; i < info.frame_count; i++)
    {
        DX12Texture* back_texture = PLACEMENT_NEW(DX12Texture, sizeof(DX12Texture));
        back_texture->m_resource = buffers[i];
        back_texture->m_ref_device = device;
        // texture info will be freed automatically in ~DX12Texture
        back_texture->m_info = Allocator<DX12Texture::GPUTextureInfo>::allocate(1);
        back_texture->m_info->is_cube = false;
        back_texture->m_info->array_layers = 1;
        back_texture->m_info->sample_count = GPUSampleCount::e_1; // TODO: ?
        back_texture->m_info->format = info.format;
        back_texture->m_info->aspect_mask = 1;
        back_texture->m_info->depth = 1;
        back_texture->m_info->width = info.width;
        back_texture->m_info->height = info.height;
        back_texture->m_info->mip_levels = 1;
        back_texture->m_info->node_index = GPU_Node_Index;
        back_texture->m_info->owns_image = false;

        GPUTextureViewCreateInfo view_info{
            .texture = back_texture,
            .format = info.format,
            .usage = GPUTextureViewUsageFlag::e_rtv_dsv,
            .aspect = GPUTextureViewAspectFlag::e_color,
            .type = GPUTextureType::e_2d,
            .array_layers = 1,
        };

        DX12TextureView* back_texture_view = PLACEMENT_NEW(DX12TextureView, sizeof(DX12TextureView), view_info);

        m_back_textures[i] = { back_texture, back_texture_view };
    }

    m_ref_device = device;
}

DX12SwapChain::~DX12SwapChain()
{
    for (size_t i = 0; i < m_back_textures.size(); i++)
    {
        DX12Texture* back_texture = static_cast<DX12Texture*>(m_back_textures[i].back_texture);
        DX12TextureView* back_texture_view = static_cast<DX12TextureView*>(m_back_textures[i].back_texture_view);
        PLACEMENT_DELETE(DX12TextureView, back_texture_view);
        PLACEMENT_DELETE(DX12Texture, back_texture);
    }
    DX_FREE(m_swap_chain);
}

uint32_t DX12SwapChain::acquire_next_frame(GPUSemaphore const* semaphore, GPUFence* fence)
{
    return m_swap_chain->GetCurrentBackBufferIndex();
}

GPUTexture const* DX12SwapChain::fetch_back_texture(uint32_t index) const
{
    return m_back_textures[index].back_texture;
}

GPUTextureView const* DX12SwapChain::fetch_back_texture_view(uint32_t index) const
{
    return m_back_textures[index].back_texture_view;
}


AMAZING_NAMESPACE_END