//
// Created by AmazingBuff on 2025/7/7.
//

#ifndef BUFFER_VIEW_H
#define BUFFER_VIEW_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUBufferView
{
public:
    GPUBufferView() : m_ref_buffer(nullptr), m_usage(GPUBufferViewUsage::e_cbv), m_format(GPUFormat::e_undefined) {}
    virtual ~GPUBufferView() = default;

protected:
    GPUBuffer const* m_ref_buffer;
    GPUBufferViewUsage m_usage;
    GPUFormat           m_format;
};

AMAZING_NAMESPACE_END

#endif //BUFFER_VIEW_H
