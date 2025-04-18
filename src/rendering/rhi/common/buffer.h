//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef BUFFER_H
#define BUFFER_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUBuffer
{
public:
    GPUBuffer() = default;
    virtual ~GPUBuffer() = 0;

    virtual AResult initialize() = 0;
};

AMAZING_NAMESPACE_END

#endif //BUFFER_H
