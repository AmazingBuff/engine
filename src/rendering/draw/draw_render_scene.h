//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef DRAW_RENDER_SCENE_H
#define DRAW_RENDER_SCENE_H

#include "rendering/render_entity.h"
#include "rendering/draw/render_scene.h"
#include "rendering/acceleration/render_command.h"
#include "rendering/graph/resource/render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraphPassNode;
class RenderGraphResourceNode;

class DrawRenderScene final : public RenderScene
{
public:
    explicit DrawRenderScene(RenderSceneCreateInfo const& info);
    ~DrawRenderScene() override = default;

    void add_entity(RenderEntity const& entity) override;
    void remove_entity(RenderEntity const& entity) override;
    void attach_graph(RenderGraph const* graph) override;
    void attach_view(RenderView const* view) override;
    void render() override;
private:
    void render_graphics(RenderGraphPassNode* node, RenderGraphPipeline const* pipeline);
    void render_compute(RenderGraphPassNode* node, RenderGraphPipeline const* pipeline);
private:
    HashSet<RenderEntity> m_render_entities;
};


AMAZING_NAMESPACE_END
#endif //DRAW_RENDER_SCENE_H
