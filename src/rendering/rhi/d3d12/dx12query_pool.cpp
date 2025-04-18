//
// Created by AmazingBuff on 2025/4/18.
//

#include "dx12query_pool.h"
#include "dx12device.h"
#include "rendering/api.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

static constexpr D3D12_QUERY_TYPE Query_Type_Map[GPU_Query_Type_Count] =
{
    D3D12_QUERY_TYPE_OCCLUSION,
    D3D12_QUERY_TYPE_TIMESTAMP,
    D3D12_QUERY_TYPE_PIPELINE_STATISTICS
};

static constexpr D3D12_QUERY_HEAP_TYPE Query_Heap_Type_Map[GPU_Query_Type_Count] =
{
    D3D12_QUERY_HEAP_TYPE_OCCLUSION,
    D3D12_QUERY_HEAP_TYPE_TIMESTAMP,
    D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS
};


DX12QueryPool::DX12QueryPool() : m_query_heap(nullptr), m_type(D3D12_QUERY_TYPE_TIMESTAMP) {}

DX12QueryPool::~DX12QueryPool()
{
    DX_FREE(m_query_heap);
}

AResult DX12QueryPool::initialize(GPUDevice const* device, GPUQueryPoolCreateInfo const& info)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);

    D3D12_QUERY_HEAP_DESC desc{
        .Type = Query_Heap_Type_Map[std::to_underlying(info.query_type)],
        .Count = info.query_count,
        .NodeMask = GPU_Node_Mask
    };

    DX_CHECK_RESULT(dx12_device->m_device->CreateQueryHeap(&desc, IID_PPV_ARGS(&m_query_heap)));
    m_type = Query_Type_Map[std::to_underlying(info.query_type)];

    return AResult::e_succeed;
}


AMAZING_NAMESPACE_END
