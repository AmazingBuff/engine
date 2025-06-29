//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_system.h"
#include "../../../include/rendering/render_entity.h"
#include "rendering/acceleration/render_driver.h"
#include "rendering/acceleration/render_geometry.h"
#include "rendering/graph/resource/render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

static RenderEntity generate_render_entity()
{
    static std::atomic<uint32_t> id = 1;
    RenderEntity ret(id.load(std::memory_order_acquire));
    ++id;
    return ret;
}

DrawRenderSystem::DrawRenderSystem(RenderSystemCreateInfo const& info)
{
    RenderDriverCreateInfo driver_info{
        .backend = info.backend,
        .frame_count = RENDER_Frame_Count,
    };
    m_render_driver = PLACEMENT_NEW(RenderDriver, sizeof(RenderDriver), driver_info);
}

DrawRenderSystem::~DrawRenderSystem()
{
    for (auto& [entity, geometry] : m_render_geometries)
        m_render_driver->destroy_render_geometry(geometry);
    for (auto& [entity, pipeline] : m_render_graph_pipelines)
        m_render_driver->destroy_pipeline(pipeline);

    PLACEMENT_DELETE(RenderDriver, const_cast<RenderDriver*>(m_render_driver));
}

RenderEntity DrawRenderSystem::import_scene(Scene const& scene)
{
    RenderGeometry geometry = m_render_driver->import_render_geometry(scene);
    RenderEntity entity = generate_render_entity();
    m_render_geometries[entity] = std::move(geometry);
    return entity;
}

RenderEntity DrawRenderSystem::import_scene(const char* file)
{
    RenderGeometry geometry = m_render_driver->import_render_geometry(file);
    RenderEntity entity = generate_render_entity();
    m_render_geometries[entity] = std::move(geometry);
    return entity;
}

RenderEntity DrawRenderSystem::create_pipeline(RenderGraphPipelineCreateInfo const& info)
{
    RenderGraphPipeline pipeline = m_render_driver->create_pipeline(info);
    RenderEntity entity = generate_render_entity();
    m_render_graph_pipelines[entity] = std::move(pipeline);
    return entity;
}

RenderEntity DrawRenderSystem::create_image(RenderGraphImageCreateInfo const& info)
{
    RenderEntity entity = generate_render_entity();
    return entity;
}

AMAZING_NAMESPACE_END