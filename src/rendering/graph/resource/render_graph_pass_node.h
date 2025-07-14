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
    RenderGeometry const* m_ref_render_geometry;
    RenderEntity m_geometry_entity;

    RenderGraphPassExecute m_execute;
    HashMap<RenderGraphResourceNode const*, RenderGraphResourceBarrier> m_input_barriers;
    HashMap<RenderGraphResourceNode const*, RenderGraphResourceBarrier> m_output_barriers;

    struct RenderDescriptorResource
    {
        struct RenderDescriptorData : GPUDescriptorData
        {
            union
            {
                GPUTexture const* const* textures;
                GPUBuffer const* const* buffers;
            };
        };

        Vector<RenderDescriptorData> descriptor_data;
        GPUDescriptorSet* descriptor_set;
    };

    HashMap<uint32_t, RenderDescriptorResource> m_descriptor_resources;


    friend class DrawRenderBuilder;
    friend class DrawRenderGraph;
    friend class DrawRenderScene;
    friend class DrawRenderGraphicsView;
    friend class DrawRenderComputeView;
};

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_PASS_NODE_H
