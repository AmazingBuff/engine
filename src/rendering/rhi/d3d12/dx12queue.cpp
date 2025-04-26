//
// Created by AmazingBuff on 2025/4/16.
//

#include "dx12queue.h"
#include "dx12device.h"
#include "dx12fence.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12Queue::DX12Queue() : m_queue(nullptr), m_fence(nullptr) {}

DX12Queue::~DX12Queue()
{
    Allocator<DX12Fence>::deallocate(m_fence);
    DX_FREE(m_queue);
}


AMAZING_NAMESPACE_END