//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_VIEW_H
#define RENDER_VIEW_H

#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

// a structure which control render behavior
class RenderView
{
public:
    RenderView() = default;
    virtual ~RenderView() = default;

    virtual void set_uniform(String const& name, void const* data) = 0;
    virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) = 0;
    virtual void set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void set_push_constant(String const& name, void const* data) = 0;
    virtual void dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;
};



AMAZING_NAMESPACE_END
#endif //RENDER_VIEW_H
