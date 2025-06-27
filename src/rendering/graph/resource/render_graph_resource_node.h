//
// Created by AmazingBuff on 2025/6/26.
//

#ifndef RENDER_GRAPH_RESOURCE_NODE_H
#define RENDER_GRAPH_RESOURCE_NODE_H

#include "render_graph_resources.h"
#include "rendering/render_entity.h"
#include "core/dependency/dependency_node.h"

AMAZING_NAMESPACE_BEGIN

// the input of resource node must be equal to 1
class RenderGraphResourceNode final : public DependencyNode
{
public:
    explicit RenderGraphResourceNode(RenderGraphResourceIOType io_type);
    ~RenderGraphResourceNode() override = default;

    void attach_entity(RenderEntity const& entity);
    void insert_barrier(RenderGraphResourceBarrier const& barrier);
private:
    RenderEntity m_resource_entity;
    RenderGraphResourceIOType m_io_type;
    RenderGraphResourceBarrier m_barrier;

    friend class DrawRenderScene;
};


AMAZING_NAMESPACE_END
#endif //RENDER_GRAPH_RESOURCE_NODE_H
