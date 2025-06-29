//
// Created by AmazingBuff on 25-6-29.
//

#ifndef RENDER_GRAPH_NODE_H
#define RENDER_GRAPH_NODE_H

#include "rendering/render_entity.h"
#include "core/dependency/dependency_node.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraphNode : public DependencyNode
{
public:
    RenderGraphNode() = default;
    ~RenderGraphNode() override = default;

    void attach_entity(RenderEntity const& entity) { m_entity = entity; }
    RenderEntity const& entity() const { return m_entity; }
private:
    RenderEntity m_entity;
};


AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_NODE_H
