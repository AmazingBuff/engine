//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef RENDER_ENUM_H
#define RENDER_ENUM_H

#include "core/math/algebra.h"

AMAZING_NAMESPACE_BEGIN

enum class RenderBackend : uint8_t
{
    e_d3d12 = 0,
    e_vulkan = 1,
};

enum class RenderFormat : uint8_t
{
    e_undefined,
    e_r8g8b8a8,
    e_b8g8r8a8,

    e_d32,
    e_d24s8
};



AMAZING_NAMESPACE_END
#endif //RENDER_ENUM_H
