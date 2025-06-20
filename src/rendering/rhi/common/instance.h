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

    virtual void enum_adapters(Vector<GPUAdapter*>& adapters) const = 0;
};

AMAZING_NAMESPACE_END



#endif //INSTANCE_H
