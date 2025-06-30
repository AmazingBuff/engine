//
// Created by AmazingBuff on 2025/6/26.
//

#ifndef RENDER_GRAPH_RESOURCE_EDGE_H
#define RENDER_GRAPH_RESOURCE_EDGE_H

#include "core/dependency/dependency_edge.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraphResourceEdge final : public DependencyEdge
{
public:
    explicit RenderGraphResourceEdge(GPUResourceState state) : m_state(state) {}
    NODISCARD GPUResourceState state() const { return m_state; }
private:
    GPUResourceState m_state;
};

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_RESOURCE_EDGE_H
