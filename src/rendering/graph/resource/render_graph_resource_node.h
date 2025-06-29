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
    explicit RenderGraphResourceNode(RenderGraphResource const& resource) : m_ref_resource(resource), m_barrier{} {}
    ~RenderGraphResourceNode() override = default;

    NODISCARD virtual RenderGraphResourceType type() const = 0;

    void insert_barrier(RenderGraphResourceBarrier const& barrier) { m_barrier = barrier; }
protected:
    RenderGraphResource const& m_ref_resource;
    RenderGraphResourceBarrier m_barrier;

    friend class DrawRenderScene;
};

class RenderGraphBufferNode final : public RenderGraphResourceNode
{
public:
    explicit RenderGraphBufferNode(RenderGraphResource const& resource) : RenderGraphResourceNode(resource) {}
    ~RenderGraphBufferNode() override = default;

    NODISCARD RenderGraphResourceType type() const override { return RenderGraphResourceType::e_buffer; }

    NODISCARD RenderGraphBufferUsage src_usage() const;
    NODISCARD RenderGraphBufferUsage dst_usage() const;
};

class RenderGraphImageNode final : public RenderGraphResourceNode
{
public:
    explicit RenderGraphImageNode(RenderGraphResource const& resource) : RenderGraphResourceNode(resource) {}
    ~RenderGraphImageNode() override = default;

    NODISCARD RenderGraphResourceType type() const override { return RenderGraphResourceType::e_image; }

    NODISCARD RenderGraphImageUsage src_usage() const;
    NODISCARD RenderGraphImageUsage dst_usage() const;
};

AMAZING_NAMESPACE_END
#endif //RENDER_GRAPH_RESOURCE_NODE_H
