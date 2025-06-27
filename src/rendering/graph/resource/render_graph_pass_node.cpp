//
// Created by AmazingBuff on 2025/6/26.
//

#include "render_graph_pass_node.h"

AMAZING_NAMESPACE_BEGIN

void RenderGraphPassNode::attach_pipeline(RenderEntity const& entity)
{
    if (!m_pipeline_entity.is_valid())
        m_pipeline_entity = entity;
    else
        RENDERING_LOG_WARNING("can't attach pipeline twice! pipeline id is {}", entity.id());
}

void RenderGraphPassNode::add_execute(RenderGraphPassExecute&& execute)
{
    m_execute = std::move(execute);
}


AMAZING_NAMESPACE_END