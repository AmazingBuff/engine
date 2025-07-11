//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef DRAW_RENDER_GRAPH_H
#define DRAW_RENDER_GRAPH_H

#include "rendering/graph/render_graph.h"
#include "rendering/render_entity.h"
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
    ~DrawRenderGraph() override;

    void add_pass(const char* pass_name, RenderGraphPassSetup&& setup, RenderGraphPassExecute&& execute) override;
    void add_present_pass(const char* pass_name, RenderEntity const& present_entity) override;
    void compile() override;
private:
    HashMap<String, RenderGraphPassNode*> m_pass_nodes;
    HashMap<String, RenderGraphResourceNode*> m_resource_nodes;
    HashSet<RenderGraphResourceEdge*> m_edges;

    Vector<Vector<RenderGraphPassNode*>> m_parallel_groups;

    struct
    {
        String name;
        RenderEntity present_entity;
    } m_present_pass;

    friend class DrawRenderBuilder;
    friend class DrawRenderScene;
};



AMAZING_NAMESPACE_END
#endif //DRAW_RENDER_GRAPH_H
