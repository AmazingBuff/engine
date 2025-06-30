//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_graph.h"
#include "draw_render_builder.h"
#include "resource/render_graph_pass_node.h"
#include "resource/render_graph_resource_node.h"
#include "resource/render_graph_resource_edge.h"
#include "core/dependency/dependency_edge.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderGraph::DrawRenderGraph(RenderGraphCreateInfo const& info)
{
    m_ref_render_system = info.render_system;
}

void DrawRenderGraph::add_pass(const char* pass_name, RenderGraphPassSetup&& setup, RenderGraphPassExecute&& execute)
{
    if (m_pass_nodes.find(pass_name) == m_pass_nodes.end())
    {
        RenderGraphPassNode* pass_node = PLACEMENT_NEW(RenderGraphPassNode, sizeof(RenderGraphPassNode));
        DrawRenderBuilder builder(this, pass_node);
        setup(&builder);

        pass_node->add_execute(std::move(execute));
        m_pass_nodes[pass_name] = pass_node;
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
    for (auto& [name, node] : m_resource_nodes)
    {
        RenderGraphPassNode* input_node = nullptr;
        for (DependencyEdge const* in : node->input_edges())
        {
            // skip uav edge
            if (static_cast<RenderGraphResourceEdge const*>(in)->state() != GPUResourceState::e_unordered_access)
            {
                if (input_node == nullptr)
                    input_node = static_cast<RenderGraphPassNode*>(in->from());
                else
                    RENDERING_LOG_ERROR("resource input node(exclude uav) must less than 1!");
            }
        }

        if (input_node != nullptr)
        {
            Vector<RenderGraphPassNode*> output_nodes;
            for (DependencyEdge const* out : node->output_edges())
            {
                // skip uav edge
                if (static_cast<RenderGraphResourceEdge const*>(out)->state() != GPUResourceState::e_unordered_access)
                {
                    RenderGraphPassNode* out_node = static_cast<RenderGraphPassNode*>(out->to());
                    output_nodes.push_back(out_node);
                }
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

    // insert barrier
    for (Vector<RenderGraphPassNode*> const& group : m_parallel_groups)
    {
        for (RenderGraphPassNode* node : group)
        {
            for (DependencyEdge const* in : node->input_edges())
            {
                RenderGraphResourceNode* input_node = static_cast<RenderGraphResourceNode*>(in->from());
                RenderGraphResourceEdge const* cur_edge = static_cast<RenderGraphResourceEdge const*>(in);
                RenderGraphResourceEdge const* prev_edge = nullptr;
                uint32_t node_priority = 0;
                for (DependencyEdge const* in_in : input_node->input_edges())
                {
                    RenderGraphPassNode* in_node = static_cast<RenderGraphPassNode*>(in_in->to());
                    uint32_t in_priority = priority[in_node];
                    if (in_priority > node_priority)
                    {
                        node_priority = in_priority;
                        prev_edge = static_cast<RenderGraphResourceEdge const*>(in_in);
                    }
                }
                GPUResourceState src_state = GPUResourceState::e_undefined;
                if (prev_edge != nullptr)
                    src_state = prev_edge->state();
                node->insert_barrier(input_node, {src_state, cur_edge->state()});
            }
            for (DependencyEdge const* out : node->output_edges())
            {
                RenderGraphResourceEdge const* cur_edge = static_cast<RenderGraphResourceEdge const*>(out);
                GPUResourceState dst_state = cur_edge->state();
                if (dst_state != GPUResourceState::e_unordered_access)
                {
                    RenderGraphResourceNode* output_node = static_cast<RenderGraphResourceNode*>(out->to());
                    node->insert_barrier(output_node, {GPUResourceState::e_undefined, dst_state});
                }
            }
        }
    }
}

AMAZING_NAMESPACE_END