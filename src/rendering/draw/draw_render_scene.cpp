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
#include "rendering/rhi/common/root_signature.h"

AMAZING_NAMESPACE_BEGIN

GPUResourceState transfer_resource_state(const String& name, RenderGraphPipeline const& pipeline, bool input /* or output*/)
{
    GPUResourceState state = pipeline.root_signature->fetch_shader_resource_state(name);
    if (input && state == GPUResourceState::e_undefined)
        return GPUResourceState::e_render_target;

    return state;
}


DrawRenderScene::DrawRenderScene(RenderSceneCreateInfo const& info)
    : m_render_command(static_cast<DrawRenderSystem const*>(info.render_system)->m_render_driver)
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
    DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(m_ref_render_system);
    DrawRenderGraph const* render_graph = static_cast<DrawRenderGraph const*>(graph);

    // validate resource entity
    for (auto& [name, pass_node] : render_graph->m_render_graph_passes)
    {
        if (auto it = render_system->m_render_graph_pipelines.find(pass_node->m_pipeline_entity);
            it != render_system->m_render_graph_pipelines.end() &&
            m_render_entities.find(pass_node->m_pipeline_entity) != m_render_entities.end())
            m_render_graph_pipelines[pass_node] = it->second;
        else
            RENDERING_LOG_ERROR("can't find pipeline entity in this scene! entity is {}", pass_node->m_pipeline_entity.id());
    }

    for (auto& [name, node] : render_graph->m_nodes)
    {
        RENDERING_ASSERT(node->input_edges().size() <= 1, "resource input node must less than 1!");

        // find resource
        if (auto it = render_system->m_render_graph_resources.find(node->m_resource_entity);
            it != render_system->m_render_graph_resources.end() &&
            m_render_entities.find(node->m_resource_entity) != m_render_entities.end())
            m_render_graph_resources[node] = it->second;
        else
            RENDERING_LOG_ERROR("can't find resource entity in this scene! entity is {}", node->m_resource_entity.id());

        // insert barrier
        RenderGraphPassNode* input_node = nullptr;
        for (DependencyEdge const* in : node->input_edges())
            input_node = static_cast<RenderGraphPassNode*>(in->from());

        GPUResourceState src_state = GPUResourceState::e_undefined;
        if (input_node != nullptr)
            src_state = transfer_resource_state(name, m_render_graph_pipelines[input_node], true);

        // todo: adjust state handle
        GPUResourceState dst_state = GPUResourceState::e_undefined;
        for (DependencyEdge const* out : node->output_edges())
        {
            RenderGraphPassNode* out_node = static_cast<RenderGraphPassNode*>(out->to());
            dst_state |= transfer_resource_state(name, m_render_graph_pipelines[out_node], false);
        }

        node->insert_barrier({src_state, dst_state});
    }

    m_ref_render_graph = graph;
}

void DrawRenderScene::attach_view(RenderView const* view)
{
    m_ref_render_view = view;
}

void DrawRenderScene::render()
{
    DrawRenderGraph const* render_graph = static_cast<DrawRenderGraph const*>(m_ref_render_graph);

    uint32_t frame_index = 0;
    for (auto& group : render_graph->m_parallel_groups)
    {
        for (auto& pass_node : group)
        {
            for (DependencyEdge const* in : pass_node->input_edges())
            {
                RenderGraphResourceNode* in_node = static_cast<RenderGraphResourceNode*>(in->from());
                RenderGraphResource resource = m_render_graph_resources[in_node];
                // barrier

            }


        }
    }
}


AMAZING_NAMESPACE_END