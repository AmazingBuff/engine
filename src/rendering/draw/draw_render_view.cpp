//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_view.h"
#include "rendering/rhi/wrapper.h"
#include "rendering/graph/resource/render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderGraphicsView::DrawRenderGraphicsView(RenderGraphicsCommand& command) : m_graphics_command(command) {}

void DrawRenderGraphicsView::set_viewport(float x, float y, float width, float height, float min_depth, float max_depth)
{
    m_graphics_command.m_graphics_encoder->set_viewport(x, y, width, height, min_depth, max_depth);
}

void DrawRenderGraphicsView::set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    m_graphics_command.m_graphics_encoder->set_scissor(x, y, width, height);
}

void DrawRenderGraphicsView::set_push_constant(String const& name, void const* data)
{
    m_graphics_command.m_graphics_encoder->set_push_constant(m_graphics_command.m_ref_pipeline->root_signature, name, data);
}

void DrawRenderGraphicsView::bind_scene_geometry(RenderEntity const& entity)
{
    m_geometry_entity = entity;
}


DrawRenderComputeView::DrawRenderComputeView(RenderComputeCommand& command) : m_compute_command(command) {}

void DrawRenderComputeView::set_viewport(float x, float y, float width, float height, float min_depth, float max_depth)
{
    RENDERING_LOG_ERROR("can't invoke graphics pipeline function in compute pipeline!");
}

void DrawRenderComputeView::set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    RENDERING_LOG_ERROR("can't invoke graphics pipeline function in compute pipeline!");
}

void DrawRenderComputeView::set_push_constant(String const& name, void const* data)
{
    m_compute_command.m_compute_encoder->set_push_constant(m_compute_command.m_ref_pipeline->root_signature, name, data);
}

void DrawRenderComputeView::bind_scene_geometry(RenderEntity const& entity)
{
    RENDERING_LOG_ERROR("can't invoke graphics pipeline function in compute pipeline!");
}

AMAZING_NAMESPACE_END