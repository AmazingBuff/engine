//
// Created by AmazingBuff on 2025/6/26.
//

#include "core/dependency/dependency_node.h"

AMAZING_NAMESPACE_BEGIN

void DependencyNode::add_input_edge(DependencyEdge* edge)
{
    m_in_dependency_edges.emplace(edge);
}

void DependencyNode::add_output_edge(DependencyEdge* edge)
{
    m_out_dependency_edges.emplace(edge);
}

const HashSet<DependencyEdge*>& DependencyNode::input_edges() const
{
    return m_in_dependency_edges;
}

const HashSet<DependencyEdge*>& DependencyNode::output_edges() const
{
    return m_out_dependency_edges;
}

AMAZING_NAMESPACE_END