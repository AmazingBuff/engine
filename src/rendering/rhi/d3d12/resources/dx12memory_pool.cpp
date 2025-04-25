//
// Created by AmazingBuff on 2025/4/23.
//

#include "dx12memory_pool.h"
#include "rendering/rhi/d3d12/dx12device.h"
#include "rendering/rhi/d3d12/utils/dx_macro.h"
#include "rendering/rhi/d3d12/utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DX12MemoryPool::DX12MemoryPool() : m_pool(nullptr) {}

DX12MemoryPool::~DX12MemoryPool()
{

}

AResult DX12MemoryPool::initialize(GPUDevice const* device, GPUMemoryPoolCreateInfo const& info)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);

    D3D12MA::POOL_DESC desc{
        .HeapProperties{
            .Type = transfer_heap_type(info.usage)
        },
        .BlockSize = info.block_size,
        .MinBlockCount = info.min_block_count,
        .MaxBlockCount = info.max_block_count,
        .MinAllocationAlignment = info.min_allocation_alignment
    };

    switch (info.type)
    {
    case GPUMemoryPoolType::e_tiled:
        desc.HeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
        break;
    default:
        desc.HeapFlags = D3D12_HEAP_FLAG_NONE;
        break;
    }

    DX_CHECK_RESULT(dx12_device->m_allocator->CreatePool(&desc, &m_pool));

    m_type = info.type;

    return AResult::e_succeed;
}

AMAZING_NAMESPACE_END