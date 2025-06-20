//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef DRAW_RENDER_GRAPH_H
#define DRAW_RENDER_GRAPH_H

#include "rendering/graph/render_graph.h"

AMAZING_NAMESPACE_BEGIN

class DrawRenderGraph final : public RenderGraph
{
public:
    ~DrawRenderGraph() override = default;

    void add_pass(const char* pass_name, RenderGraphPassSetup&& setup, RenderGraphPassExecute&& execute) override;
private:
    HashMap<String, RenderGraphPassSetup> m_graph_passes;
};



AMAZING_NAMESPACE_END
#endif //DRAW_RENDER_GRAPH_H
