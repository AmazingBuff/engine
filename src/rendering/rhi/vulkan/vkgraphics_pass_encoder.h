//
// Created by AmazingBuff on 2025/5/13.
//

#ifndef VKGRAPHICS_PASS_ENCODER_H
#define VKGRAPHICS_PASS_ENCODER_H

#include "vk.h"
#include "rendering/rhi/common/graphics_pass_encoder.h"

AMAZING_NAMESPACE_BEGIN

class VKCommandBuffer;

class VKGraphicsPassEncoder final : public GPUGraphicsPassEncoder
{
public:
    void bind_vertex_buffers(GPUBufferBinding const* bindings, uint32_t binding_count) override;
    void bind_index_buffer(GPUBufferBinding const& binding) override;
    void bind_descriptor_set(GPUDescriptorSet const* set) override;
    void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) override;
    void set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void bind_pipeline(GPUGraphicsPipeline const* pipeline) override;
    void set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data) override;
    void draw(uint32_t vertex_count, uint32_t first_vertex) override;
    void draw_instance(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) override;
    void draw_indexed(uint32_t index_count, uint32_t first_index, uint32_t first_vertex) override;
    void draw_indexed_instance(uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t first_vertex, uint32_t first_instance) override;
private:
    VKGraphicsPassEncoder();

private:
    VKCommandBuffer* m_command;

    friend class VKCommandBuffer;
};

AMAZING_NAMESPACE_END

#endif //VKGRAPHICS_PASS_ENCODER_H
