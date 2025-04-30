//
// Created by AmazingBuff on 2025/4/18.
//

#include "dx12query_pool.h"
#include "dx12device.h"
#include "rendering/api.h"
#include "utils/dx_macro.h"
#include "utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DX12QueryPool::DX12QueryPool(GPUDevice const* device, GPUQueryPoolCreateInfo const& info) : m_query_heap(nullptr), m_type(D3D12_QUERY_TYPE_TIMESTAMP)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);

    D3D12_QUERY_HEAP_DESC desc{
        .Type = Query_Heap_Type_Map[to_underlying(info.query_type)],
        .Count = info.query_count,
        .NodeMask = GPU_Node_Mask
    };

    DX_CHECK_RESULT(dx12_device->m_device->CreateQueryHeap(&desc, IID_PPV_ARGS(&m_query_heap)));
    m_type = Query_Type_Map[to_underlying(info.query_type)];
}

DX12QueryPool::~DX12QueryPool()
{
    DX_FREE(m_query_heap);
}


AMAZING_NAMESPACE_END
