//
// Created by AmazingBuff on 2025/6/25.
//

#include "draw_render_builder.h"
#include "rendering/graph/resource/render_graph_pass_node.h"
#include "rendering/graph/resource/render_graph_resource_node.h"
#include "rendering/graph/resource/render_graph_resource_edge.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderBuilder::DrawRenderBuilder(DrawRenderGraph* graph, RenderGraphPassNode* pass_node)
    : m_ref_render_graph(graph), m_ref_graph_pass_node(pass_node) {}

DrawRenderBuilder::~DrawRenderBuilder()
{

}

void DrawRenderBuilder::bind_pipeline(RenderEntity const& entity)
{
    m_ref_graph_pass_node->attach_pipeline(entity);
}

void DrawRenderBuilder::read(const char* name, RenderEntity const& entity)
{
    auto it = m_ref_render_graph->m_nodes.find(name);
    if (it == m_ref_render_graph->m_nodes.end())
    {
        RenderGraphResourceNode* node = PLACEMENT_NEW(RenderGraphResourceNode, sizeof(RenderGraphResourceNode), RenderGraphResourceIOType::e_read);
        node->attach_entity(entity);
        m_ref_render_graph->m_nodes.emplace(name, node);

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
    auto it = m_ref_render_graph->m_nodes.find(name);
    if (it == m_ref_render_graph->m_nodes.end())
    {
        RenderGraphResourceNode* node = PLACEMENT_NEW(RenderGraphResourceNode, sizeof(RenderGraphResourceNode), RenderGraphResourceIOType::e_write);
        node->attach_entity(entity);
        m_ref_render_graph->m_nodes.emplace(name, node);

        RenderGraphResourceEdge* edge = PLACEMENT_NEW(RenderGraphResourceEdge, sizeof(RenderGraphResourceEdge));
        edge->link(m_ref_graph_pass_node, node);
        m_ref_render_graph->m_edges.emplace(edge);
        m_ref_graph_pass_node->add_output_edge(edge);
    }
    else
    {
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

void DrawRenderBuilder::read_write(const char* name, RenderEntity const& entity)
{
    auto it = m_ref_render_graph->m_nodes.find(name);
    if (it == m_ref_render_graph->m_nodes.end())
    {
        RenderGraphResourceNode* node = PLACEMENT_NEW(RenderGraphResourceNode, sizeof(RenderGraphResourceNode), RenderGraphResourceIOType::e_read_write);
        node->attach_entity(entity);
        m_ref_render_graph->m_nodes.emplace(name, node);

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

AMAZING_NAMESPACE_END