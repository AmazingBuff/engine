//
// Created by AmazingBuff on 2025/6/26.
//

#ifndef RENDER_GRAPH_RESOURCE_NODE_H
#define RENDER_GRAPH_RESOURCE_NODE_H

#include "render_graph_node.h"
#include "render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

// the input of resource node must be equal to 1
class RenderGraphResourceNode : public RenderGraphNode
{
public:
    explicit RenderGraphResourceNode(RenderGraphResource const& resource) : m_ref_resource(resource) {}
    ~RenderGraphResourceNode() override = default;

    NODISCARD virtual RenderGraphResourceType type() const = 0;
protected:
    RenderGraphResource const& m_ref_resource;

    friend class DrawRenderScene;
    friend class DrawRenderGraph;
};

class RenderGraphBufferNode final : public RenderGraphResourceNode
{
public:
    explicit RenderGraphBufferNode(RenderGraphResource const& resource) : RenderGraphResourceNode(resource) {}
    ~RenderGraphBufferNode() override = default;

    NODISCARD RenderGraphResourceType type() const override { return RenderGraphResourceType::e_buffer; }
};

class RenderGraphImageNode final : public RenderGraphResourceNode
{
public:
    explicit RenderGraphImageNode(RenderGraphResource const& resource) : RenderGraphResourceNode(resource) {}
    ~RenderGraphImageNode() override = default;

    NODISCARD RenderGraphResourceType type() const override { return RenderGraphResourceType::e_image; }
};

AMAZING_NAMESPACE_END
#endif //RENDER_GRAPH_RESOURCE_NODE_H
