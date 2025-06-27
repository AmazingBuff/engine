//
// Created by AmazingBuff on 2025/6/25.
//

#ifndef DEPENDENCY_EDGE_H
#define DEPENDENCY_EDGE_H

#include "core/base.h"

AMAZING_NAMESPACE_BEGIN

class DependencyGraph;
class DependencyNode;

// directed acyclic graph edge
class DependencyEdge
{
public:
    DependencyEdge();
    virtual ~DependencyEdge() = default;

    void link(DependencyNode* from_node, DependencyNode* to_node);
    DependencyNode* from() const;
    DependencyNode* to() const;
protected:
    DependencyNode* m_from_node;
    DependencyNode* m_to_node;
};


AMAZING_NAMESPACE_END
#endif //DEPENDENCY_EDGE_H
