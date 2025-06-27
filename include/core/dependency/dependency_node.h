//
// Created by AmazingBuff on 2025/6/25.
//

#ifndef DEPENDENCY_NODE_H
#define DEPENDENCY_NODE_H

#include "core/base.h"

AMAZING_NAMESPACE_BEGIN

class DependencyGraph;
class DependencyEdge;

// directed acyclic graph node
class DependencyNode
{
public:
    DependencyNode() = default;
    virtual ~DependencyNode() = default;

    void add_input_edge(DependencyEdge* edge);
    void add_output_edge(DependencyEdge* edge);

    const HashSet<DependencyEdge*>& input_edges() const;
    const HashSet<DependencyEdge*>& output_edges() const;
protected:
    HashSet<DependencyEdge*> m_in_dependency_edges;
    HashSet<DependencyEdge*> m_out_dependency_edges;
};



AMAZING_NAMESPACE_END
#endif //DEPENDENCY_NODE_H
