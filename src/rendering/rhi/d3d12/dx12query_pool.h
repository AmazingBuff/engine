//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef DX12QUERY_POOL_H
#define DX12QUERY_POOL_H

#include "rendering/rhi/common/query_pool.h"

AMAZING_NAMESPACE_BEGIN

class DX12QueryPool final : public GPUQueryPool
{
public:
    DX12QueryPool(GPUDevice const* device, GPUQueryPoolCreateInfo const& info);
    ~DX12QueryPool() override;

private:
    ID3D12QueryHeap* m_query_heap;
    D3D12_QUERY_TYPE m_type;

    friend class DX12CommandBuffer;
};


AMAZING_NAMESPACE_END

#endif //DX12QUERY_POOL_H
