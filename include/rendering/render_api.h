//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_API_H
#define RENDER_API_H

#include "render_type.h"
#include "rendering/draw/render_system.h"
#include "rendering/draw/render_scene.h"
#include "rendering/draw/render_view.h"

AMAZING_NAMESPACE_BEGIN

RenderSystem* RENDER_create_render_system(RenderSystemCreateInfo const& info);
void RENDER_destroy_render_system(RenderSystem* render_system);

RenderScene* RENDER_create_render_scene(RenderSceneCreateInfo const& info);
void RENDER_destroy_render_scene(RenderScene* render_scene);

RenderView* RENDER_create_render_view(RenderViewCreateInfo const& info);
void RENDER_destroy_render_view(RenderView* render_view);


AMAZING_NAMESPACE_END

#endif //RENDER_API_H
