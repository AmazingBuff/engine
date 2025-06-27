//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef DRAW_RENDER_GRAPH_H
#define DRAW_RENDER_GRAPH_H

#include "rendering/graph/render_graph.h"
#include "rendering/draw/draw_render_system.h"
#include "core/dependency/dependency_graph.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraphPassNode;
class RenderGraphResourceNode;
class RenderGraphResourceEdge;

class DrawRenderGraph final : public RenderGraph, public DependencyGraph
{
public:
    explicit DrawRenderGraph(RenderGraphCreateInfo const& info);
    ~DrawRenderGraph() override = default;

    void add_pass(const char* pass_name, RenderGraphPassSetup&& setup, RenderGraphPassExecute&& execute) override;
    void compile() override;
private:
    HashMap<String, RenderGraphPassNode*> m_render_graph_passes;
    HashMap<String, RenderGraphResourceNode*> m_nodes;
    HashSet<RenderGraphResourceEdge*> m_edges;

    Vector<Vector<RenderGraphPassNode*>> m_parallel_groups;

    friend class DrawRenderBuilder;
    friend class DrawRenderScene;
};



AMAZING_NAMESPACE_END
#endif //DRAW_RENDER_GRAPH_H
