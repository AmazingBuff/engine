//
// Created by AmazingBuff on 2025/4/27.
//

#ifndef GRAPHICS_PASS_ENCODER_H
#define GRAPHICS_PASS_ENCODER_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUGraphicsPassEncoder
{
public:
    GPUGraphicsPassEncoder() = default;
    virtual ~GPUGraphicsPassEncoder() = default;

    virtual void bind_vertex_buffers(GPUBufferBinding const* bindings, uint32_t binding_count) = 0;
    virtual void bind_index_buffer(GPUBufferBinding const& binding) = 0;
    virtual void bind_descriptor_set(GPUDescriptorSet const* set) = 0;
    virtual void bind_pipeline(GPUGraphicsPipeline const* pipeline) = 0;
    virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) = 0;
    virtual void set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data) = 0;
    virtual void draw(uint32_t vertex_count, uint32_t first_vertex) = 0;
    virtual void draw_instance(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) = 0;
    virtual void draw_indexed(uint32_t index_count, uint32_t first_index, uint32_t first_vertex) = 0;
    virtual void draw_indexed_instance(uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t first_vertex, uint32_t first_instance) = 0;
};

AMAZING_NAMESPACE_END

#endif //GRAPHICS_PASS_ENCODER_H
