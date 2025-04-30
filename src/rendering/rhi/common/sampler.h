//
// Created by AmazingBuff on 2025/4/22.
//

#ifndef SAMPLER_H
#define SAMPLER_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUSampler
{
public:
    GPUSampler() : m_ref_device(nullptr) {}
    virtual ~GPUSampler() = default;

protected:
    GPUDevice const* m_ref_device;
};

AMAZING_NAMESPACE_END

#endif //SAMPLER_H
