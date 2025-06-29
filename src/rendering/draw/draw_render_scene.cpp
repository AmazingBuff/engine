//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_scene.h"
#include "draw_render_system.h"
#include "core/dependency/dependency_edge.h"
#include "rendering/acceleration/render_driver.h"
#include "rendering/graph/draw_render_graph.h"
#include "rendering/graph/resource/render_graph_pass_node.h"
#include "rendering/graph/resource/render_graph_resource_node.h"

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

void DrawRenderScene::attach_graph(RenderGraph const* graph)
{
    DrawRenderGraph const* render_graph = static_cast<DrawRenderGraph const*>(graph);

    // validate entity
    for_each(render_graph->m_pass_nodes, [this](const Pair<String, RenderGraphPassNode*>& node)
    {
        RenderEntity const& entity = node.second->entity();
        if (m_render_entities.find(entity) == m_render_entities.end())
            RENDERING_LOG_ERROR("can't find pipeline entity in this scene! entity is {}", entity.id());
    });

    for_each(render_graph->m_resource_nodes, [this](const Pair<String, RenderGraphResourceNode*>& node)
    {
        RenderEntity const& entity = node.second->entity();
        if (m_render_entities.find(entity) == m_render_entities.end())
            RENDERING_LOG_ERROR("can't find pipeline entity in this scene! entity is {}", entity.id());
    });

    m_ref_render_graph = graph;
}

void DrawRenderScene::attach_view(RenderView const* view)
{
    m_ref_render_view = view;
}

void DrawRenderScene::render()
{
    DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(m_ref_render_system);
    DrawRenderGraph const* render_graph = static_cast<DrawRenderGraph const*>(m_ref_render_graph);
    RenderDriver const* render_driver = render_system->m_render_driver;

    uint32_t frame_index = 0;
    for (auto& group : render_graph->m_parallel_groups)
    {
        for (auto& pass_node : group)
        {
            RenderGraphPipeline const* pipeline = pass_node->m_ref_pipeline;
            switch (pipeline->pipeline_type)
            {
            case GPUPipelineType::e_graphics:
                render_graphics(pass_node, pipeline);
                break;
            case GPUPipelineType::e_compute:
                render_compute(pass_node, pipeline);
                break;
            default:
                break;
            }
        }
    }
}

void DrawRenderScene::render_graphics(RenderGraphPassNode* node, RenderGraphPipeline const* pipeline)
{
    DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(m_ref_render_system);
    RenderDriver const* render_driver = render_system->m_render_driver;

    RenderGraphicsCommand command(render_driver);
    command.begin_frame();

    uint32_t node_count = node->input_edges().size();
    RenderGraphResource* resources = STACK_NEW(RenderGraphResource, node_count);
    RenderGraphResourceBarrier* barriers = STACK_NEW(RenderGraphResourceBarrier, node_count);

    uint32_t node_index = 0;
    for (DependencyEdge const* in : node->input_edges())
    {
        RenderGraphResourceNode* in_node = static_cast<RenderGraphResourceNode*>(in->from());
        resources[node_index] = in_node->m_ref_resource;
        barriers[node_index] = in_node->m_barrier;
        node_index++;
    }

    command.resource_barrier(resources, barriers, node_count);

    // get render target
    for (DependencyEdge const* out : node->output_edges())
    {
        RenderGraphResourceNode* out_node = static_cast<RenderGraphResourceNode*>(out->to());

    }


    command.end_frame();
}


AMAZING_NAMESPACE_END