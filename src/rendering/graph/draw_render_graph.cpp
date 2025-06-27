//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_graph.h"
#include "draw_render_builder.h"
#include "resource/render_graph_pass_node.h"
#include "core/dependency/dependency_edge.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderGraph::DrawRenderGraph(RenderGraphCreateInfo const& info)
{
    m_ref_render_system = info.render_system;
}

void DrawRenderGraph::add_pass(const char* pass_name, RenderGraphPassSetup&& setup, RenderGraphPassExecute&& execute)
{
    if (m_render_graph_passes.find(pass_name) == m_render_graph_passes.end())
    {
        RenderGraphPassNode* pass_node = PLACEMENT_NEW(RenderGraphPassNode, sizeof(RenderGraphPassNode));
        DrawRenderBuilder builder(this, pass_node);
        setup(&builder);

        pass_node->add_execute(std::move(execute));
        m_render_graph_passes[pass_name] = pass_node;
    }
}

void DrawRenderGraph::compile()
{
    struct PassNodeEdge
    {
        HashSet<RenderGraphPassNode*> input_nodes;
        HashSet<RenderGraphPassNode*> output_nodes;
    };

    HashMap<RenderGraphPassNode*, PassNodeEdge> pass_graph;
    // get render pass node dependency graph
    // the prev and next of resource node must be render pass node
    for (auto& [name, node] : m_nodes)
    {
        RENDERING_ASSERT(node->input_edges().size() <= 1, "resource input node must less than 1!");

        RenderGraphPassNode* input_node = nullptr;
        for (DependencyEdge const* in : node->input_edges())
            input_node = static_cast<RenderGraphPassNode*>(in->from());

        if (input_node != nullptr)
        {
            Vector<RenderGraphPassNode*> output_nodes;
            for (DependencyEdge const* out : node->output_edges())
            {
                RenderGraphPassNode* out_node = static_cast<RenderGraphPassNode*>(out->to());
                output_nodes.push_back(out_node);
            }

            for (RenderGraphPassNode* out_node : output_nodes)
            {
                pass_graph[input_node].output_nodes.insert(out_node);
                pass_graph[out_node].input_nodes.insert(input_node);
            }
        }
    }

    // get the priority of each render node
    Queue<RenderGraphPassNode*> start_nodes;
    HashMap<RenderGraphPassNode*, uint32_t> priority;
    HashMap<RenderGraphPassNode*, uint32_t> in_degrees;
    for_each(pass_graph, [&](const Pair<RenderGraphPassNode*, PassNodeEdge>& pass_node)
    {
        if (pass_node.second.input_nodes.empty())
        {
            start_nodes.enqueue(pass_node.first);
            priority[pass_node.first] = 0;
        }
        else
            in_degrees[pass_node.first] = pass_node.second.input_nodes.size();
    });

    uint32_t max_priority = 0;
    while (!start_nodes.empty())
    {
        RenderGraphPassNode* node = start_nodes.front();
        for (RenderGraphPassNode* out_node : pass_graph[node].output_nodes)
        {
            in_degrees[out_node]--;
            auto it = priority.find(out_node);
            if (it == priority.end())
                priority[out_node] = priority[node] + 1;
            else
                it->second = std::max(priority[node] + 1, it->second);

            if (in_degrees[out_node] == 0)
                start_nodes.enqueue(out_node);

            max_priority = std::max(priority[out_node], max_priority);
        }
        start_nodes.dequeue();
    }

    m_parallel_groups.resize(max_priority + 1);
    for (auto& [node, priority] : priority)
        m_parallel_groups[priority].push_back(node);

    // todo: cull pass
}

AMAZING_NAMESPACE_END