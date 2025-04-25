//
// Created by AmazingBuff on 2025/4/23.
//

#ifndef DX12MEMORY_POOL_H
#define DX12MEMORY_POOL_H

#include "rendering/rhi/common/memory_pool.h"
#include "rendering/rhi/d3d12/d3d12ma/D3D12MemAlloc.h"

AMAZING_NAMESPACE_BEGIN

class DX12MemoryPool final : public GPUMemoryPool
{
public:
    DX12MemoryPool();
    ~DX12MemoryPool() override;


    AResult initialize(GPUDevice const* device, GPUMemoryPoolCreateInfo const& info) override;

private:
    D3D12MA::Pool* m_pool;
};

AMAZING_NAMESPACE_END

#endif //DX12MEMORY_POOL_H
