//
// Created by AmazingBuff on 2025/4/11.
//

#include "dx12swapchain.h"
#include "dx12instance.h"
#include "dx12device.h"
#include "dx12queue.h"
#include "rendering/api.h"
#include "utils/dx_macro.h"
#include "utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DX12SwapChain::DX12SwapChain() : m_swap_chain(nullptr) {}

DX12SwapChain::~DX12SwapChain()
{
    for (size_t i = 0; i < m_swap_chain_buffer.size(); i++)
        DX_FREE(m_swap_chain_buffer[i].resource);
    DX_FREE(m_swap_chain);
}

AResult DX12SwapChain::initialize(GPUInstance const* instance, GPUDevice const* device, GPUSwapChainCreateInfo const& info)
{
    DX12Instance const* dx12_instance = static_cast<DX12Instance const*>(instance);
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);

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

    }

    IDXGISwapChain1* swap_chain;
    HWND hwnd = reinterpret_cast<HWND>(info.surface);

    DX12Queue* queue = nullptr;
    if (info.present_queues.empty())
    {
        queue = Allocator<DX12Queue>::allocate(1);
        queue->initialize(dx12_device, GPUQueueType::e_graphics, 0);
    }
    else
        queue = static_cast<DX12Queue*>(info.present_queues[0]);

    DX_CHECK_RESULT(dx12_instance->m_dxgi_factory->CreateSwapChainForHwnd(queue->m_queue, hwnd, &swap_chain_desc, nullptr, nullptr, &swap_chain));
    DX_CHECK_RESULT(dx12_instance->m_dxgi_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
    DX_CHECK_RESULT(swap_chain->QueryInterface(IID_PPV_ARGS(&m_swap_chain)));

    DX_FREE(swap_chain);

    ID3D12Resource** buffers = static_cast<ID3D12Resource**>(alloca(info.frame_count * sizeof(ID3D12Resource*)));
    for (uint32_t i = 0; i < info.frame_count; i++)
        DX_CHECK_RESULT(m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&buffers[i])));

    m_swap_chain_buffer.resize(info.frame_count);
    for (uint32_t i = 0; i < info.frame_count; i++)
    {
        m_swap_chain_buffer[i].resource = buffers[i];
        m_swap_chain_buffer[i].tex_info.is_cube              = false;
        m_swap_chain_buffer[i].tex_info.array_size_minus_one = 0;
        m_swap_chain_buffer[i].tex_info.sample_count         = GPUSampleCount::e_1; // TODO: ?
        m_swap_chain_buffer[i].tex_info.format               = info.format;
        m_swap_chain_buffer[i].tex_info.aspect_mask          = 1;
        m_swap_chain_buffer[i].tex_info.depth                = 1;
        m_swap_chain_buffer[i].tex_info.width                = info.width;
        m_swap_chain_buffer[i].tex_info.height               = info.height;
        m_swap_chain_buffer[i].tex_info.mip_levels           = 1;
        m_swap_chain_buffer[i].tex_info.node_index           = GPU_Node_Count;
        m_swap_chain_buffer[i].tex_info.owns_image           = false;
    }

    return AResult::e_succeed;
}

uint32_t DX12SwapChain::acquire_next_frame(GPUSemaphore const* semaphore, GPUFence const* fence)
{
    return m_swap_chain->GetCurrentBackBufferIndex();
}

AMAZING_NAMESPACE_END