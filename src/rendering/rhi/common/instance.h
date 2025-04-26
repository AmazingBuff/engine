//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef INSTANCE_H
#define INSTANCE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUInstance
{
public:
    GPUInstance() = default;
    virtual ~GPUInstance() = default;

    virtual AResult initialize(GPUInstanceCreateInfo const& info) = 0;
    virtual void enum_adapters(const GPUAdapter** const adapters, uint32_t* num_adapters) const = 0;
};

AMAZING_NAMESPACE_END



#endif //INSTANCE_H
