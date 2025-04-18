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
    GPUDevice() = default;
    virtual ~GPUDevice() = 0;

    virtual AResult initialize(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info) = 0;
};

AMAZING_NAMESPACE_END

#endif //DEVICE_H
