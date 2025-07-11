//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_view.h"
#include "draw_render_scene.h"
#include "draw_render_system.h"
#include "rendering/render_util.h"
#include "rendering/rhi/wrapper.h"
#include "rendering/graph/resource/render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderGraphicsView::DrawRenderGraphicsView(DrawRenderScene const* scene, RenderGraphicsCommand& command) : m_graphics_command(command)
{
    m_ref_render_scene = scene;
}

void DrawRenderGraphicsView::set_uniform(RenderEntity const& entity, uint32_t offset, uint32_t size, void const* data)
{
    DrawRenderScene const* render_scene = static_cast<DrawRenderScene const*>(m_ref_render_scene);

    auto it = render_scene->m_render_entities.find(entity);
    if (it != render_scene->m_render_entities.end())
    {
        DrawRenderSystem const* render_system = static_cast<DrawRenderSystem const*>(render_scene->m_ref_render_system);
        RenderGraphResource const& resource = render_system->m_render_graph_resources[entity];

        switch (resource.resource_type)
        {
        case RenderGraphResourceType::e_buffer:
        {
            RENDERING_ASSERT(resource.buffer.buffer->descriptor()->type == GPUResourceType::e_uniform_buffer, "unmatched buffer type!");
            resource.buffer.buffer->map(offset, size, data);
        }
            break;
        case RenderGraphResourceType::e_image:
            RENDERING_LOG_ERROR("unsupported resource type!");
            break;
        }
    }
    else
        RENDERING_LOG_ERROR("the entity {} isn't imported to this scene!", entity.id());
}

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


DrawRenderComputeView::DrawRenderComputeView(DrawRenderScene const* scene, RenderComputeCommand& command) : m_compute_command(command)
{
    m_ref_render_scene = scene;
}

void DrawRenderComputeView::set_uniform(RenderEntity const& entity, uint32_t offset, uint32_t size, void const* data)
{

}

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

AMAZING_NAMESPACE_END