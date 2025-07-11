//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef DRAW_RENDER_SYSTEM_H
#define DRAW_RENDER_SYSTEM_H

#include "rendering/draw/render_system.h"
#include "rendering/acceleration/render_command.h"

AMAZING_NAMESPACE_BEGIN

struct RenderGeometry;
struct RenderGraphResource;
struct RenderGraphPipeline;

class DrawRenderSystem final : public RenderSystem
{
public:
    explicit DrawRenderSystem(RenderSystemCreateInfo const& info);
    ~DrawRenderSystem() override;

    RenderEntity import_scene(Scene const& scene) override;
    RenderEntity import_scene(const char *file) override;

    RenderEntity create_pipeline(RenderGraphPipelineCreateInfo const& info) override;
    RenderEntity create_image(RenderGraphImageCreateInfo const& info) override;
    RenderEntity create_buffer(RenderGraphBufferCreateInfo const& info) override;
private:
    RenderDriver m_render_driver;
    RenderGraphicsCommand m_graphics_command;
    RenderComputeCommand m_compute_command;

    HashMap<RenderEntity, RenderGeometry> m_render_geometries;
    HashMap<RenderEntity, RenderGraphPipeline> m_render_graph_pipelines;
    HashMap<RenderEntity, RenderGraphResource> m_render_graph_resources;

    friend class DrawRenderScene;
    friend class DrawRenderBuilder;
    friend class DrawRenderGraphicsView;
};

AMAZING_NAMESPACE_END
#endif //DRAW_RENDER_SYSTEM_H
