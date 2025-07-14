//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_system.h"
#include "rendering/render_util.h"
#include "rendering/render_entity.h"
#include "rendering/acceleration/render_driver.h"
#include "rendering/acceleration/render_geometry.h"
#include "rendering/graph/resource/render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderSystem::DrawRenderSystem(RenderSystemCreateInfo const& info) :
    m_render_driver({info.backend, RENDER_Frame_Count, info.window_handle, info.window_width, info.window_height, info.format}),
    m_graphics_command(m_render_driver), m_compute_command(m_render_driver) {}

DrawRenderSystem::~DrawRenderSystem()
{
    for (auto& [entity, geometry] : m_render_geometries)
        m_render_driver.destroy_render_geometry(geometry);
    for (auto& [entity, pipeline] : m_render_graph_pipelines)
        m_render_driver.destroy_pipeline(pipeline);
}

RenderEntity DrawRenderSystem::import_scene(Scene const& scene)
{
    RenderGeometry geometry = m_render_driver.import_render_geometry(scene);
    RenderEntity entity = generate_render_entity();
    m_render_geometries[entity] = std::move(geometry);
    return entity;
}

RenderEntity DrawRenderSystem::import_scene(const char* file)
{
    RenderGeometry geometry = m_render_driver.import_render_geometry(file);
    RenderEntity entity = generate_render_entity();
    m_render_geometries[entity] = std::move(geometry);
    return entity;
}

RenderEntity DrawRenderSystem::create_pipeline(RenderGraphPipelineCreateInfo const& info)
{
    RenderGraphPipeline pipeline = m_render_driver.create_pipeline(info);
    RenderEntity entity = generate_render_entity();
    m_render_graph_pipelines[entity] = pipeline;
    return entity;
}

RenderEntity DrawRenderSystem::create_image(RenderGraphImageCreateInfo const& info)
{
    RenderGraphResource image = m_render_driver.create_image(info);
    RenderEntity entity = generate_render_entity();
    m_render_graph_resources[entity] = image;
    return entity;
}

RenderEntity DrawRenderSystem::create_buffer(RenderGraphBufferCreateInfo const& info)
{
    RenderGraphResource buffer = m_render_driver.create_buffer(info);
    RenderEntity entity = generate_render_entity();
    m_render_graph_resources[entity] = buffer;
    return entity;
}

AMAZING_NAMESPACE_END