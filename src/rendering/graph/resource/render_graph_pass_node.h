//
// Created by AmazingBuff on 2025/6/26.
//

#ifndef RENDER_GRAPH_PASS_NODE_H
#define RENDER_GRAPH_PASS_NODE_H

#include "render_graph_node.h"
#include "render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraphPassNode final : public RenderGraphNode
{
public:
    RenderGraphPassNode() : m_ref_pipeline(nullptr), m_ref_render_geometry(nullptr) {}
    ~RenderGraphPassNode() override = default;
private:
    RenderGraphPipeline const* m_ref_pipeline;
    RenderGraphPassExecute m_execute;
    HashMap<RenderGraphResourceNode const*, RenderGraphResourceBarrier> m_input_barriers;
    HashMap<RenderGraphResourceNode const*, RenderGraphResourceBarrier> m_output_barriers;
    HashMap<uint32_t, Vector<String>> m_descriptors;
    HashMap<uint32_t, GPUDescriptorSet*> m_descriptor_sets;

    RenderEntity m_geometry_entity;
    RenderGeometry const* m_ref_render_geometry;

    friend class DrawRenderBuilder;
    friend class DrawRenderGraph;
    friend class DrawRenderScene;
};

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_PASS_NODE_H
