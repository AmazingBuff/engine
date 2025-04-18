//
// Created by AmazingBuff on 2025/4/11.
//

#include "dx12fence.h"
#include "dx12device.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12Fence::DX12Fence() : m_fence(nullptr), m_fence_value(0), m_wait_event(nullptr) {}

DX12Fence::~DX12Fence()
{
    CloseHandle(m_wait_event);
    DX_FREE(m_fence);
}

AResult DX12Fence::initialize(GPUDevice const* device)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    DX_CHECK_RESULT(dx12_device->m_device->CreateFence(m_fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

    m_wait_event = CreateEvent(nullptr, 0, 0, nullptr);

    return AResult::e_succeed;
}

AResult DX12Fence::wait()
{
    if (m_fence->GetCompletedValue() < m_fence_value)
    {
        DX_CHECK_RESULT(m_fence->SetEventOnCompletion(m_fence_value, m_wait_event));
        WaitForSingleObject(m_wait_event, INFINITE);
    }

    return AResult::e_succeed;
}

AMAZING_NAMESPACE_END
