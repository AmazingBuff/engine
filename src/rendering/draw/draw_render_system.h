//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef DRAW_RENDER_SYSTEM_H
#define DRAW_RENDER_SYSTEM_H

#include "rendering/draw/render_system.h"

AMAZING_NAMESPACE_BEGIN

struct RenderGeometry;
struct RenderGraphResource;
struct RenderGraphPipeline;
class RenderDriver;

class DrawRenderSystem final : public RenderSystem
{
public:
    explicit DrawRenderSystem(RenderSystemCreateInfo const& info);
    ~DrawRenderSystem() override;

    RenderEntity import_scene(Scene const& scene) override;
    RenderEntity import_scene(const char *file) override;

    RenderEntity create_pipeline(RenderGraphPipelineCreateInfo const& info) override;
    RenderEntity create_image(RenderGraphImageCreateInfo const& info) override;
private:
    RenderDriver const* m_render_driver;

    HashMap<RenderEntity, RenderGeometry> m_render_geometries;
    HashMap<RenderEntity, RenderGraphPipeline> m_render_graph_pipelines;
    HashMap<RenderEntity, RenderGraphResource> m_render_graph_resources;

    friend class DrawRenderScene;
    friend class DrawRenderBuilder;
};

AMAZING_NAMESPACE_END
#endif //DRAW_RENDER_SYSTEM_H
