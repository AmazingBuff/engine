//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef FENCE_H
#define FENCE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUFence
{
public:
    GPUFence() : m_ref_device(nullptr) {}
    virtual ~GPUFence() = default;

    virtual void wait() = 0;
protected:
    GPUDevice const* m_ref_device;
};

AMAZING_NAMESPACE_END

#endif //FENCE_H
