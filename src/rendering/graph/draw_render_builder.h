//
// Created by AmazingBuff on 2025/6/25.
//

#ifndef DRAW_RENDER_BUILDER_H
#define DRAW_RENDER_BUILDER_H

#include "draw_render_graph.h"
#include "rendering/graph/render_builder.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraphPassNode;

class DrawRenderBuilder final : public RenderBuilder
{
public:
    DrawRenderBuilder(DrawRenderGraph* graph, RenderGraphPassNode* pass_node);
    ~DrawRenderBuilder() override;

    void bind_pipeline(RenderEntity const& entity) override;
    void read(const char* name, RenderEntity const& entity) override;
    void write(const char* name, RenderEntity const& entity) override;
    void read_write(const char* name, RenderEntity const& entity) override;
private:
    DrawRenderGraph* m_ref_render_graph;
    RenderGraphPassNode* m_ref_graph_pass_node;
};

AMAZING_NAMESPACE_END

#endif //DRAW_RENDER_BUILDER_H
