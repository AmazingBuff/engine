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
    GPUFence() = default;
    virtual ~GPUFence() = 0;

    virtual AResult initialize(GPUDevice const* device) = 0;
    virtual AResult wait() = 0;
};

AMAZING_NAMESPACE_END

#endif //FENCE_H
