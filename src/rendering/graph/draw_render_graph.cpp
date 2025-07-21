//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_graph.h"
#include "draw_render_builder.h"
#include "resource/render_graph_pass_node.h"
#include "resource/render_graph_resource_node.h"
#include "resource/render_graph_resource_edge.h"
#include "core/dependency/dependency_edge.h"
#include "rendering/rhi/wrapper.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderGraph::DrawRenderGraph(RenderGraphCreateInfo const& info)
{
    m_ref_render_system = info.render_system;
}

DrawRenderGraph::~DrawRenderGraph()
{
    for (auto& [name, node] : m_pass_nodes)
    {
        for (auto& [set_index, descriptor_resource] : node->m_descriptor_resources)
            GPU_destroy_descriptor_set(descriptor_resource.descriptor_set);
        PLACEMENT_DELETE(RenderGraphPassNode, node);
    }

    for (auto& [name, node] : m_resource_nodes)
    {
        PLACEMENT_DELETE(RenderGraphResourceNode, node);
    }

    for_each(m_edges, [](RenderGraphResourceEdge* edge)
    {
        PLACEMENT_DELETE(RenderGraphResourceEdge, edge);
    });
}

void DrawRenderGraph::add_pass(const char* pass_name, RenderGraphPassSetup&& setup, RenderGraphPassExecute&& execute)
{
    if (m_pass_nodes.find(pass_name) == m_pass_nodes.end())
    {
        RenderGraphPassNode* pass_node = PLACEMENT_NEW(RenderGraphPassNode, sizeof(RenderGraphPassNode));
        DrawRenderBuilder builder(this, pass_node);
        setup(builder);

        pass_node->m_execute = execute;
        m_pass_nodes[pass_name] = pass_node;
    }
}

void DrawRenderGraph::add_present_pass(const char* pass_name, RenderEntity const& present_entity)
{
    m_present_pass.name = pass_name;
    m_present_pass.present_entity = present_entity;
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

            PassNodeEdge edge;
            pass_graph.emplace(input_node, edge);
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

    RENDERING_ASSERT(m_parallel_groups[max_priority].size() == 1, "last pass of rendering must only has one output!");

    // todo: cull pass

    for (Vector<RenderGraphPassNode*> const& group : m_parallel_groups)
    {
        for (RenderGraphPassNode* node : group)
        {
            // insert barrier
            for (DependencyEdge const* in : node->input_edges())
            {
                RenderGraphResourceNode* input_node = static_cast<RenderGraphResourceNode*>(in->from());
                RenderGraphResourceEdge const* cur_edge = static_cast<RenderGraphResourceEdge const*>(in);
                RenderGraphResourceEdge const* prev_edge = nullptr;
                uint32_t node_priority = 0;
                for (DependencyEdge const* in_in : input_node->input_edges())
                {
                    RenderGraphPassNode* in_node = static_cast<RenderGraphPassNode*>(in_in->from());
                    // add dependency
                    node->m_dependency_nodes.push_back(in_node);
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
                node->m_input_barriers[input_node] = {src_state, cur_edge->state()};
            }
            for (DependencyEdge const* out : node->output_edges())
            {
                RenderGraphResourceEdge const* cur_edge = static_cast<RenderGraphResourceEdge const*>(out);
                GPUResourceState dst_state = cur_edge->state();
                if (dst_state != GPUResourceState::e_unordered_access)
                {
                    RenderGraphResourceNode* output_node = static_cast<RenderGraphResourceNode*>(out->to());

                    GPUResourceState src_state = GPUResourceState::e_undefined;
                    switch (output_node->m_ref_resource.resource_type)
                    {
                    case RenderGraphResourceType::e_image:
                        src_state = output_node->m_ref_resource.image.texture->description()->state;
                        break;
                    default:
                        break;
                    }
                    node->m_output_barriers[output_node] = {src_state, dst_state};
                }
            }

            // update resource
            for_each(node->m_descriptor_resources, [&](Pair<uint32_t, RenderGraphPassNode::RenderDescriptorResource>& descriptor)
            {
                GPUDescriptorSetCreateInfo desc{
                    .root_signature = node->m_ref_pipeline->root_signature,
                    .set_index = descriptor.first,
                };
                GPUDescriptorSet* descriptor_set = GPU_create_descriptor_set(desc);

                for_each(descriptor.second.descriptor_data, [&](RenderGraphPassNode::RenderDescriptorResource::RenderDescriptorData& descriptor_data)
                {
                    auto resource_it = m_resource_nodes.find(descriptor_data.name);
                    if (resource_it != m_resource_nodes.end())
                    {
                        RenderGraphResource const& graph_resource = resource_it->second->m_ref_resource;

                        switch (resource_it->second->type())
                        {
                        case RenderGraphResourceType::e_image:
                            descriptor_data.texture_views = &graph_resource.image.texture_view;
                            descriptor_data.textures = &graph_resource.image.texture;
                            break;
                        case RenderGraphResourceType::e_buffer:
                            descriptor_data.buffer_views = &graph_resource.buffer.buffer_view;
                            descriptor_data.buffers = &graph_resource.buffer.buffer;
                            break;
                        }
                    }
                });
                descriptor_set->update(descriptor.second.descriptor_data.data(), descriptor.second.descriptor_data.size());
                descriptor.second.descriptor_set = descriptor_set;
            });
        }
    }
}

AMAZING_NAMESPACE_END