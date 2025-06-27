//
// Created by AmazingBuff on 2025/6/26.
//

#include "core/dependency/dependency_edge.h"

AMAZING_NAMESPACE_BEGIN

DependencyEdge::DependencyEdge() : m_from_node(nullptr), m_to_node(nullptr) {}

void DependencyEdge::link(DependencyNode* from_node, DependencyNode* to_node)
{
    m_from_node = from_node;
    m_to_node = to_node;
}

DependencyNode* DependencyEdge::from() const
{
    return m_from_node;
}

DependencyNode* DependencyEdge::to() const
{
    return m_to_node;
}

AMAZING_NAMESPACE_END