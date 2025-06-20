//
// Created by AmazingBuff on 2025/6/17.
//

#include "rendering/render_api.h"
#include "rendering/draw/draw_render_system.h"
#include "rendering/draw/draw_render_scene.h"
#include "rendering/draw/draw_render_view.h"

AMAZING_NAMESPACE_BEGIN

RenderSystem* RENDER_create_render_system(RenderSystemCreateInfo const& info)
{
    return PLACEMENT_NEW(DrawRenderSystem, sizeof(DrawRenderSystem), info);
}

void RENDER_destroy_render_system(RenderSystem* render_system)
{
    PLACEMENT_DELETE(DrawRenderSystem, static_cast<DrawRenderSystem*>(render_system));
}

RenderScene* RENDER_create_render_scene(RenderSceneCreateInfo const& info)
{
    return PLACEMENT_NEW(DrawRenderScene, sizeof(DrawRenderScene), info);
}
void RENDER_destroy_render_scene(RenderScene* render_scene)
{
    PLACEMENT_DELETE(DrawRenderScene, static_cast<DrawRenderScene*>(render_scene));
}

RenderView* RENDER_create_render_view(RenderViewCreateInfo const& info)
{
    return PLACEMENT_NEW(DrawRenderView, sizeof(DrawRenderView), info);
}
void RENDER_destroy_render_view(RenderView* render_view)
{
    PLACEMENT_DELETE(DrawRenderView, static_cast<DrawRenderView*>(render_view));
}


AMAZING_NAMESPACE_END