//
// Created by AmazingBuff on 2025/4/22.
//

#include "dx12sampler.h"
#include "rendering/rhi/d3d12/dx12device.h"
#include "rendering/rhi/d3d12/internal/dx12descriptor_heap.h"
#include "rendering/rhi/d3d12/utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DX12Sampler::DX12Sampler(GPUDevice const* device, GPUSamplerCreateInfo const& info) : m_desc{}, m_handle{}
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);

    D3D12_SAMPLER_DESC sampler_desc{
        .Filter = transfer_filter(info.min_filter, info.mag_filter, info.mipmap_mode, info.max_anisotropy > 0.f, info.compare_mode != GPUCompareMode::e_never),
        .AddressU = transfer_address_mode(info.address_u),
        .AddressV = transfer_address_mode(info.address_v),
        .AddressW = transfer_address_mode(info.address_w),
        .MipLODBias = info.mip_lod_bias,
        .MaxAnisotropy = std::max(static_cast<uint32_t>(info.max_anisotropy), 1u),
        .ComparisonFunc = Compare_Mode_Map[to_underlying(info.compare_mode)],
        .BorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
        .MinLOD = 0.0f,
        .MaxLOD = info.mipmap_mode == GPUMipMapMode::e_linear ? std::numeric_limits<float>::max() : 0.0f,
    };

    DX12DescriptorHeap::D3D12DescriptorHeap* heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER];
    m_handle = DX12DescriptorHeap::consume_descriptor_handle(heap, 1).cpu;

    dx12_device->m_device->CreateSampler(&sampler_desc, m_handle);

    m_ref_device = device;
    m_desc = sampler_desc;
}

DX12Sampler::~DX12Sampler()
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(m_ref_device);

    DX12DescriptorHeap::D3D12DescriptorHeap* heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER];
    DX12DescriptorHeap::return_descriptor_handle(heap, m_handle, 1);
}


AMAZING_NAMESPACE_END
