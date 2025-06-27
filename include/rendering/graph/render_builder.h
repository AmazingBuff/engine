//
// Created by AmazingBuff on 2025/6/25.
//

#ifndef RENDER_BUILDER_H
#define RENDER_BUILDER_H
#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

class RenderBuilder
{
public:
    virtual ~RenderBuilder() = default;

    virtual void bind_pipeline(RenderEntity const& entity) = 0;
    virtual void read(const char* name, RenderEntity const& entity) = 0;
    virtual void write(const char* name, RenderEntity const& entity) = 0;
    virtual void read_write(const char* name, RenderEntity const& entity) = 0;
};

AMAZING_NAMESPACE_END
#endif //RENDER_BUILDER_H
