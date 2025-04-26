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
    GPUSemaphore() = default;
    virtual ~GPUSemaphore() = default;

    virtual AResult initialize(GPUDevice const* device) = 0;
};

AMAZING_NAMESPACE_END

#endif //SEMAPHORE_H
