//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_view.h"
#include "draw_render_scene.h"
#include "draw_render_system.h"
#include "rendering/render_util.h"
#include "rendering/rhi/wrapper.h"
#include "rendering/graph/resource/render_graph_resources.h"
#include "rendering/graph/resource/render_graph_pass_node.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderGraphicsView::DrawRenderGraphicsView(RenderGraphPassNode const* node, RenderGraphicsCommand& command)
    : m_ref_pass_node(node), m_graphics_command(command) {}

void DrawRenderGraphicsView::set_uniform(String const& name, void const* data)
{
    for (auto& [set_index, descriptor_resource] : m_ref_pass_node->m_descriptor_resources)
    {
        if (any_of(descriptor_resource.descriptor_data, [&](RenderGraphPassNode::RenderDescriptorResource::RenderDescriptorData const& descriptor)
        {
            if (descriptor.name == name)
            {
                RENDERING_ASSERT(descriptor.resource_type == GPUResourceType::e_uniform_buffer, "can only set uniform buffer object!");

                descriptor.buffers[0]->map(0, descriptor.buffers[0]->description()->size, data);
                descriptor_resource.descriptor_set->update(&descriptor, 1);

                return true;
            }
            return false;
        }))
            break;
    }
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

void DrawRenderGraphicsView::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    RENDERING_LOG_ERROR("can't invoke compute pipeline function in graphics pipeline!");
}



DrawRenderComputeView::DrawRenderComputeView(RenderGraphPassNode const* node, RenderComputeCommand& command)
    : m_ref_pass_node(node), m_compute_command(command) {}

void DrawRenderComputeView::set_uniform(String const& name, void const* data)
{
    for (auto& [set_index, descriptor_resource] : m_ref_pass_node->m_descriptor_resources)
    {
        if (any_of(descriptor_resource.descriptor_data, [&](RenderGraphPassNode::RenderDescriptorResource::RenderDescriptorData const& descriptor)
        {
            if (descriptor.name == name)
            {
                RENDERING_ASSERT(descriptor.resource_type == GPUResourceType::e_uniform_buffer, "can only set uniform buffer object!");

                descriptor.buffers[0]->map(0, descriptor.buffers[0]->description()->size, data);
                descriptor_resource.descriptor_set->update(&descriptor, 1);

                return true;
            }
            return false;
        }))
            break;
    }
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

void DrawRenderComputeView::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    m_compute_command.m_compute_encoder->dispatch(x, y, z);
}

AMAZING_NAMESPACE_END