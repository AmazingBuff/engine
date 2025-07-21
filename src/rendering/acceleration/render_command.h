//
// Created by AmazingBuff on 2025/6/27.
//

#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "render_driver.h"
#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

struct RenderGraphResource;
struct RenderGraphResourceBarrier;


struct RenderCommandSubmitInfo
{
    Vector<GPUSemaphore const*> wait_semaphores;
    Vector<GPUSemaphore*> signal_semaphores;
    GPUFence* signal_fence;
};

class RenderCommand
{
public:
    explicit RenderCommand(RenderDriver const& driver);
    virtual ~RenderCommand() = default;

    void begin_frame();
    void end_frame();
    void resource_barrier(RenderGraphResource const* resources, RenderGraphResourceBarrier const* info, uint32_t count) const;
    // copy the data of src_resource to dst_resource
    void copy_resource(RenderGraphResource const& src_resource, RenderGraphResource const& dst_resource) const;

    virtual void submit(RenderCommandSubmitInfo const& info) = 0;
protected:
    void initialize_command(GPUQueue const* queue);
    void refresh_frame();
protected:
    RenderDriver const& m_ref_driver;
    RenderGraphPipeline const* m_ref_pipeline;
    uint32_t m_frame_index;
    uint32_t m_frame_count;

    Vector<GPUCommandPool*> m_command_pools;
    Vector<GPUCommandBuffer*> m_command_buffers;
};

class RenderGraphicsCommand final : public RenderCommand
{
public:
    explicit RenderGraphicsCommand(RenderDriver const& driver);
    ~RenderGraphicsCommand() override;

    void submit(RenderCommandSubmitInfo const& info) override;

    void begin_pass(GPUGraphicsPassCreateInfo const& info);
    void end_pass();

    void bind_descriptor_set(GPUDescriptorSet const* set) const;
    void bind_pipeline(RenderGraphPipeline const* pipeline);
    void bind_vertex_buffers(GPUBufferBinding const* bindings, uint32_t count) const;
    void bind_index_buffer(GPUBufferBinding const& binding) const;
    void draw(uint32_t index_count, uint32_t first_index, uint32_t first_vertex) const;
private:
    GPUGraphicsPassEncoder* m_graphics_encoder;

    friend class DrawRenderGraphicsView;
};

class RenderComputeCommand final : public RenderCommand
{
public:
    explicit RenderComputeCommand(RenderDriver const& driver);
    ~RenderComputeCommand() override;

    void bind_descriptor_set(GPUDescriptorSet const* set) const;
    void bind_pipeline(RenderGraphPipeline const* pipeline);
    void begin_pass(GPUComputePassCreateInfo const& info);
    void end_pass();
    void submit(RenderCommandSubmitInfo const& info) override;
private:
    GPUComputePassEncoder* m_compute_encoder;

    friend class DrawRenderComputeView;
};

AMAZING_NAMESPACE_END

#endif //RENDER_COMMAND_H
