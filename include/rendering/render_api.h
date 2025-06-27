//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_API_H
#define RENDER_API_H

#include "render_type.h"
#include "render_entity.h"
#include "draw/render_system.h"
#include "draw/render_scene.h"
#include "draw/render_view.h"
#include "graph/render_graph.h"
#include "graph/render_builder.h"

AMAZING_NAMESPACE_BEGIN

RenderSystem* RENDER_create_render_system(RenderSystemCreateInfo const& info);
void RENDER_destroy_render_system(RenderSystem* render_system);

RenderScene* RENDER_create_render_scene(RenderSceneCreateInfo const& info);
void RENDER_destroy_render_scene(RenderScene* render_scene);

RenderView* RENDER_create_render_view(RenderViewCreateInfo const& info);
void RENDER_destroy_render_view(RenderView* render_view);

RenderGraph* RENDER_create_render_graph(RenderGraphCreateInfo const& info);
void RENDER_destroy_render_graph(RenderGraph* render_graph);

AMAZING_NAMESPACE_END

#endif //RENDER_API_H
