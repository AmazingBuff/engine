//
// Created by AmazingBuff on 2025/6/25.
//

#ifndef DEPENDENCY_GRAPH_H
#define DEPENDENCY_GRAPH_H

#include "core/base.h"

AMAZING_NAMESPACE_BEGIN

class DependencyNode;
class DependencyEdge;

// directed acyclic graph
class DependencyGraph
{
public:
    DependencyGraph() = default;
    virtual ~DependencyGraph() = default;
};

AMAZING_NAMESPACE_END

#endif //DEPENDENCY_GRAPH_H
