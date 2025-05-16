//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef DX12QUERY_POOL_H
#define DX12QUERY_POOL_H

#include "dx12.h"
#include "rendering/rhi/common/query_pool.h"

AMAZING_NAMESPACE_BEGIN

class DX12QueryPool final : public GPUQueryPool
{
public:
    DX12QueryPool(GPUDevice const* device, GPUQueryPoolCreateInfo const& info);
    ~DX12QueryPool() override;

private:
    ID3D12QueryHeap* m_query_heap;

    friend class DX12CommandBuffer;
};


AMAZING_NAMESPACE_END

#endif //DX12QUERY_POOL_H
