//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

using RenderGraphPassSetup = Functional<void(RenderSystem*)>;
using RenderGraphPassExecute = Functional<void(RenderView*)>;

class RenderGraph
{
public:
    virtual ~RenderGraph() = default;

    virtual void add_pass(const char* pass_name, RenderGraphPassSetup&& setup, RenderGraphPassExecute&& execute) = 0;
};

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_H
