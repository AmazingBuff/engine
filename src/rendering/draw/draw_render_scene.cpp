//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_scene.h"
#include "draw_render_system.h"
#include "draw_render_view.h"
#include "core/dependency/dependency_edge.h"
#include "rendering/acceleration/render_driver.h"
#include "rendering/graph/draw_render_graph.h"
#include "rendering/graph/resource/render_graph_pass_node.h"
#include "rendering/graph/resource/render_graph_resource_node.h"
#include "rendering/graph/resource/render_graph_resource_edge.h"

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
                render_graphics(pass_node);
                break;
            case GPUPipelineType::e_compute:
                render_compute(pass_node);
                break;
            default:
                break;
            }
        }
    }
}

void DrawRenderScene::render_graphics(RenderGraphPassNode* node)
{
    DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(m_ref_render_system);
    RenderDriver const* render_driver = render_system->m_render_driver;

    RenderGraphicsCommand command(render_driver);
    command.begin_frame();

    uint32_t node_count = node->input_edges().size();
    RenderGraphResource* resources = STACK_NEW(RenderGraphResource, node_count);
    RenderGraphResourceBarrier* barriers = STACK_NEW(RenderGraphResourceBarrier, node_count);

    uint32_t node_index = 0;
    for (auto& [resource_node, barrier] : node->m_barriers)
    {
        resources[node_index] = resource_node->m_ref_resource;
        barriers[node_index] = barrier;
        node_index++;
    }

    command.resource_barrier(resources, barriers, node_count);

    // get render target
    Vector<RenderGraphResourceNode*> render_target_nodes;
    for (DependencyEdge const* out : node->output_edges())
    {
        RenderGraphResourceEdge const* edge = static_cast<RenderGraphResourceEdge const*>(out);
        if (edge->state() == GPUResourceState::e_render_target)
            render_target_nodes.push_back(static_cast<RenderGraphResourceNode*>(out->to()));
    }

    // todo: add depth attachment
    GPUGraphicsPassCreateInfo graphics_pass_create_info{
        .sample_count = GPUSampleCount::e_1,
        .depth_stencil_attachment = nullptr
    };

    graphics_pass_create_info.color_attachment_count = render_target_nodes.size();
    for (uint32_t i = 0; i < render_target_nodes.size(); i++)
    {
        GPUColorAttachment color_attachment{
            .texture_view = render_target_nodes[i]->m_ref_resource.image.texture_view,
            .resolve_view = nullptr,
            .load = GPULoadAction::e_clear,
            .store = GPUStoreAction::e_store,
            .clear_color = {0, 0, 0, 1}
        };

        graphics_pass_create_info.color_attachments[i] = color_attachment;
    }

    command.bind_pipeline(node->m_ref_pipeline->graphics_pipeline);

    RenderViewCreateInfo view_create_info{
        .render_system = render_system
    };
    DrawRenderView view(view_create_info);

    node->m_execute(&view);

    command.end_frame();
}


AMAZING_NAMESPACE_END