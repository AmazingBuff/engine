//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef DRAW_RENDER_SCENE_H
#define DRAW_RENDER_SCENE_H

#include "rendering/draw/render_entity.h"
#include "rendering/draw/render_scene.h"

AMAZING_NAMESPACE_BEGIN

class DrawRenderScene final : public RenderScene
{
public:
    explicit DrawRenderScene(RenderSceneCreateInfo const& info);
    ~DrawRenderScene() override = default;

    void add_entity(RenderEntity const& entity) override;
    void remove_entity(RenderEntity const& entity) override;
private:
    HashSet<RenderEntity> m_render_entities;
};


AMAZING_NAMESPACE_END
#endif //DRAW_RENDER_SCENE_H
