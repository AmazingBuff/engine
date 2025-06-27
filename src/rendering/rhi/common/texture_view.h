//
// Created by AmazingBuff on 2025/4/22.
//

#ifndef TEXTURE_VIEW_H
#define TEXTURE_VIEW_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUTextureView
{
public:
    GPUTextureView() : m_ref_texture(nullptr), m_usage(GPUTextureViewUsage::e_srv), m_format(GPUFormat::e_undefined) {}
    virtual ~GPUTextureView() = default;

protected:
    GPUTexture const* m_ref_texture;
    GPUTextureViewUsage m_usage;
    GPUFormat           m_format;
};

AMAZING_NAMESPACE_END

#endif //TEXTURE_VIEW_H
