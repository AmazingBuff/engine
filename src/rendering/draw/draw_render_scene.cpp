//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_scene.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderScene::DrawRenderScene(RenderSceneCreateInfo const& info)
{

    m_ref_render_system = info.render_system;
}

void DrawRenderScene::add_entity(RenderEntity const& entity)
{
    m_render_entities.insert(entity);
}

void DrawRenderScene::remove_entity(RenderEntity const& entity)
{
    m_render_entities.erase(entity);
}



AMAZING_NAMESPACE_END