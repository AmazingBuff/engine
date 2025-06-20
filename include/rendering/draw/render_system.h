//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "render_entity.h"
#include "io/loader/scene_loader.h"
#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

// a structure which assist RenderView render RenderScene
class RenderSystem
{
public:
    virtual ~RenderSystem() = default;

    virtual RenderEntity import_scene(Scene const& scene) = 0;
    virtual RenderEntity import_scene(const char* file) = 0;

    virtual RenderEntity create_texture(RenderGraphTextureCreateInfo const& info) = 0;
};



AMAZING_NAMESPACE_END
#endif //RENDER_SYSTEM_H
