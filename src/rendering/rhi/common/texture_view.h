//
// Created by AmazingBuff on 2025/4/22.
//

#ifndef TEXTURE_VIEW_H
#define TEXTURE_VIEW_H

#include "device.h"
#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUTextureView
{
public:
    GPUTextureView() : m_ref_device(nullptr), m_usage(GPUTextureViewUsageFlag::e_srv), m_format(GPUFormat::e_undefined) {}
    virtual ~GPUTextureView() = 0;

    virtual AResult initialize(GPUDevice const* device, GPUTextureViewCreateInfo const& info) = 0;
protected:
    GPUDevice const*    m_ref_device;
    GPUTextureViewUsage m_usage;
    GPUFormat           m_format;
};

AMAZING_NAMESPACE_END

#endif //TEXTURE_VIEW_H
