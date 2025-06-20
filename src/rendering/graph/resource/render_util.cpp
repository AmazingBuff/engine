//
// Created by AmazingBuff on 2025/6/18.
//

#include "render_util.h"

AMAZING_NAMESPACE_BEGIN

GPUFormat transfer_format(RenderFormat format)
{
    switch (format)
    {
    case RenderFormat::e_b8g8r8a8: return GPUFormat::e_b8g8r8a8_snorm;
    case RenderFormat::e_r8g8b8a8: return GPUFormat::e_r8g8b8a8_snorm;
    case RenderFormat::e_d32: return GPUFormat::e_d32_sfloat;
    case RenderFormat::e_d24s8: return GPUFormat::e_d24_unorm_s8_uint;
    default: return GPUFormat::e_undefined;
    }
}


AMAZING_NAMESPACE_END