//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUSemaphore
{
public:
    GPUSemaphore() : m_ref_device(nullptr) {}
    virtual ~GPUSemaphore() = default;
protected:
    GPUDevice const* m_ref_device;
};

AMAZING_NAMESPACE_END

#endif //SEMAPHORE_H
