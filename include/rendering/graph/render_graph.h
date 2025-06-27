//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

class RenderGraph
{
public:
    RenderGraph() : m_ref_render_system(nullptr) {}
    virtual ~RenderGraph() = default;

    virtual void add_pass(const char* pass_name, RenderGraphPassSetup&& setup, RenderGraphPassExecute&& execute) = 0;
    virtual void compile() = 0;
protected:
    RenderSystem const* m_ref_render_system;
};

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_H
