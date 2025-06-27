//
// Created by AmazingBuff on 2025/6/26.
//

#include "render_graph_resource_node.h"

AMAZING_NAMESPACE_BEGIN

RenderGraphResourceNode::RenderGraphResourceNode(RenderGraphResourceIOType io_type)
    : m_io_type(io_type), m_barrier{} {}

void RenderGraphResourceNode::attach_entity(RenderEntity const& entity)
{
    m_resource_entity = entity;
}

void RenderGraphResourceNode::insert_barrier(RenderGraphResourceBarrier const& barrier)
{
    m_barrier = barrier;
}

AMAZING_NAMESPACE_END