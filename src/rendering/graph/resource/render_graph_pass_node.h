//
// Created by AmazingBuff on 2025/6/26.
//

#ifndef RENDER_GRAPH_PASS_NODE_H
#define RENDER_GRAPH_PASS_NODE_H

#include "rendering/render_entity.h"
#include "core/dependency/dependency_node.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraphPassNode final : public DependencyNode
{
public:
    RenderGraphPassNode() = default;
    ~RenderGraphPassNode() override = default;

    void attach_pipeline(RenderEntity const& entity);
    void add_execute(RenderGraphPassExecute&& execute);
private:
    RenderEntity m_pipeline_entity;
    RenderGraphPassExecute m_execute;

    friend class DrawRenderScene;
};

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_PASS_NODE_H
