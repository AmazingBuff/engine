//
// Created by AmazingBuff on 2025/4/11.
//

#ifndef DEVICE_H
#define DEVICE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUDevice
{
public:
    GPUDevice() : m_ref_adapter(nullptr) {}
    virtual ~GPUDevice() = default;

    virtual GPUQueue const* fetch_queue(GPUQueueType type, uint32_t index) const = 0;
protected:
    GPUAdapter const* m_ref_adapter;
};

AMAZING_NAMESPACE_END

#endif //DEVICE_H
