//
// Created by AmazingBuff on 2025/6/25.
//

#include "draw_render_builder.h"
#include "rendering/draw/draw_render_system.h"
#include "rendering/graph/resource/render_graph_pass_node.h"
#include "rendering/graph/resource/render_graph_buffer_node.h"
#include "rendering/graph/resource/render_graph_image_node.h"
#include "rendering/graph/resource/render_graph_resource_edge.h"
#include "rendering/rhi/common/buffer.h"

AMAZING_NAMESPACE_BEGIN
    DrawRenderBuilder::DrawRenderBuilder(DrawRenderGraph* graph, RenderGraphPassNode* pass_node)
    : m_ref_render_graph(graph), m_ref_graph_pass_node(pass_node) {}

DrawRenderBuilder::~DrawRenderBuilder()
{

}

void DrawRenderBuilder::bind_pipeline(RenderEntity const& entity)
{
    DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(m_ref_render_graph->m_ref_render_system);

    if (auto it = render_system->m_render_graph_pipelines.find(entity); it != render_system->m_render_graph_pipelines.end())
        m_ref_graph_pass_node->m_ref_pipeline = &it->second;
    else
        RENDERING_LOG_ERROR("can't find pipeline entity! the entity is {}", entity.id());

    m_ref_graph_pass_node->attach_entity(entity);
}

void DrawRenderBuilder::read(const char* name, RenderEntity const& entity)
{
    auto it = m_ref_render_graph->m_resource_nodes.find(name);
    if (it == m_ref_render_graph->m_resource_nodes.end())
    {
        RenderGraphResourceNode* node = nullptr;

        DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(m_ref_render_graph->m_ref_render_system);
        if (auto resource_it = render_system->m_render_graph_resources.find(entity); resource_it != render_system->m_render_graph_resources.end())
        {
            RenderGraphResource const& resource = resource_it->second;
            switch (resource.resource_type)
            {
            case RenderGraphResourceType::e_buffer:
                // todo: add srv check
                node = PLACEMENT_NEW(RenderGraphBufferCbvNode, sizeof(RenderGraphBufferCbvNode), resource);
                break;
            case RenderGraphResourceType::e_image:
                node = PLACEMENT_NEW(RenderGraphImageSrvNode, sizeof(RenderGraphImageSrvNode), resource);
                break;
            }
        }
        else
            RENDERING_LOG_ERROR("unregistered render entity! the entity is {}", entity.id());

        node->attach_entity(entity);
        m_ref_render_graph->m_resource_nodes.emplace(name, node);

        RenderGraphResourceEdge* edge = PLACEMENT_NEW(RenderGraphResourceEdge, sizeof(RenderGraphResourceEdge));
        edge->link(node, m_ref_graph_pass_node);
        m_ref_render_graph->m_edges.emplace(edge);
        m_ref_graph_pass_node->add_input_edge(edge);
    }
    else
    {
        if (all_of(m_ref_render_graph->m_edges, [&](DependencyEdge const* edge)
        {
            if (edge->from() == it->second && edge->to() == m_ref_graph_pass_node)
                return false;
            return true;
        }))
        {
            RenderGraphResourceEdge* edge = PLACEMENT_NEW(RenderGraphResourceEdge, sizeof(RenderGraphResourceEdge));
            edge->link(it->second, m_ref_graph_pass_node);
            m_ref_render_graph->m_edges.emplace(edge);
            m_ref_graph_pass_node->add_input_edge(edge);
        }
    }
}

void DrawRenderBuilder::write(const char* name, RenderEntity const& entity)
{
    auto it = m_ref_render_graph->m_resource_nodes.find(name);
    if (it == m_ref_render_graph->m_resource_nodes.end())
    {
        RenderGraphResourceNode* node = nullptr;

        DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(m_ref_render_graph->m_ref_render_system);
        if (auto resource_it = render_system->m_render_graph_resources.find(entity); resource_it != render_system->m_render_graph_resources.end())
        {
            RenderGraphResource const& resource = resource_it->second;
            switch (resource.resource_type)
            {
            case RenderGraphResourceType::e_buffer:
                RENDERING_LOG_ERROR("unsupported buffer for write! the entity is {}", entity.id());
                break;
            case RenderGraphResourceType::e_image:
                node = PLACEMENT_NEW(RenderGraphImageRtvNode, sizeof(RenderGraphImageRtvNode), resource);
                break;
            }
        }
        else
            RENDERING_LOG_ERROR("unregistered render entity! the entity is {}", entity.id());

        node->attach_entity(entity);
        m_ref_render_graph->m_resource_nodes.emplace(name, node);

        RenderGraphResourceEdge* edge = PLACEMENT_NEW(RenderGraphResourceEdge, sizeof(RenderGraphResourceEdge));
        edge->link(m_ref_graph_pass_node, node);
        m_ref_render_graph->m_edges.emplace(edge);
        m_ref_graph_pass_node->add_output_edge(edge);
    }
    else
        RENDERING_LOG_ERROR("unsupported behavior! an entity can't be wrote twice! the entity is {}", entity.id());
}

void DrawRenderBuilder::read_write(const char* name, RenderEntity const& entity)
{
    auto it = m_ref_render_graph->m_resource_nodes.find(name);
    if (it == m_ref_render_graph->m_resource_nodes.end())
    {
        RenderGraphResourceNode* node = nullptr;

        DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(m_ref_render_graph->m_ref_render_system);
        if (auto resource_it = render_system->m_render_graph_resources.find(entity); resource_it != render_system->m_render_graph_resources.end())
        {
            RenderGraphResource const& resource = resource_it->second;
            switch (resource.resource_type)
            {
            case RenderGraphResourceType::e_buffer:
                node = PLACEMENT_NEW(RenderGraphBufferUavNode, sizeof(RenderGraphBufferUavNode), resource);
                break;
            case RenderGraphResourceType::e_image:
                node = PLACEMENT_NEW(RenderGraphImageUavNode, sizeof(RenderGraphImageUavNode), resource);
                break;
            }
        }
        else
            RENDERING_LOG_ERROR("unregistered render entity! the entity is {}", entity.id());

        node->attach_entity(entity);
        m_ref_render_graph->m_resource_nodes.emplace(name, node);

        RenderGraphResourceEdge* from_edge = PLACEMENT_NEW(RenderGraphResourceEdge, sizeof(RenderGraphResourceEdge));
        from_edge->link(node, m_ref_graph_pass_node);
        m_ref_render_graph->m_edges.emplace(from_edge);
        m_ref_graph_pass_node->add_input_edge(from_edge);

        RenderGraphResourceEdge* to_edge = PLACEMENT_NEW(RenderGraphResourceEdge, sizeof(RenderGraphResourceEdge));
        to_edge->link(m_ref_graph_pass_node, node);
        m_ref_render_graph->m_edges.emplace(to_edge);
        m_ref_graph_pass_node->add_output_edge(to_edge);
    }
    else
    {
        if (all_of(m_ref_render_graph->m_edges, [&](DependencyEdge const* edge)
        {
            if (edge->from() == it->second && edge->to() == m_ref_graph_pass_node)
                return false;
            return true;
        }))
        {
            RenderGraphResourceEdge* edge = PLACEMENT_NEW(RenderGraphResourceEdge, sizeof(RenderGraphResourceEdge));
            edge->link(it->second, m_ref_graph_pass_node);
            m_ref_render_graph->m_edges.emplace(edge);
            m_ref_graph_pass_node->add_input_edge(edge);
        }

        if (all_of(m_ref_render_graph->m_edges, [&](DependencyEdge const* edge)
        {
            if (edge->from() == m_ref_graph_pass_node && edge->to() == it->second)
                return false;
            return true;
        }))
        {
            RenderGraphResourceEdge* edge = PLACEMENT_NEW(RenderGraphResourceEdge, sizeof(RenderGraphResourceEdge));
            edge->link(m_ref_graph_pass_node, it->second);
            m_ref_render_graph->m_edges.emplace(edge);
            m_ref_graph_pass_node->add_output_edge(edge);
        }
    }
}

AMAZING_NAMESPACE_END