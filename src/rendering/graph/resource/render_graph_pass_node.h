//
// Created by AmazingBuff on 2025/6/26.
//

#ifndef RENDER_GRAPH_PASS_NODE_H
#define RENDER_GRAPH_PASS_NODE_H

#include "render_graph_node.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraphPassNode final : public RenderGraphNode
{
public:
    RenderGraphPassNode() : m_ref_pipeline(nullptr) {}
    ~RenderGraphPassNode() override = default;

    void add_execute(RenderGraphPassExecute&& execute);
private:
    RenderGraphPipeline const* m_ref_pipeline;
    RenderGraphPassExecute m_execute;

    friend class DrawRenderBuilder;
    friend class DrawRenderGraph;
    friend class DrawRenderScene;
};

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_PASS_NODE_H
