//
// Created by AmazingBuff on 2025/6/27.
//

#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

struct RenderGraphResource;
struct RenderGraphResourceBarrier;
class RenderDriver;


struct RenderCommandSubmitInfo
{
    Vector<GPUSemaphore const*> wait_semaphores;
    Vector<GPUSemaphore*> signal_semaphores;
    GPUFence* signal_fence;
};

class RenderCommand
{
public:
    RenderCommand() : m_frame_index(0), m_frame_count(0) {}
    virtual ~RenderCommand() = default;

    void begin_frame();
    void end_frame();
    void resource_barrier(RenderGraphResource const* resources, RenderGraphResourceBarrier const* info, uint32_t count);

    virtual void begin_pass() = 0;
    virtual void end_pass() = 0;
    virtual void submit(RenderCommandSubmitInfo const& info) = 0;
protected:
    void initialize_command(GPUQueue const* queue);
    void refresh_frame();
protected:
    RenderDriver const* m_ref_driver;
    uint32_t m_frame_index;
    uint32_t m_frame_count;

    Vector<GPUCommandPool*> m_command_pools;
    Vector<GPUCommandBuffer*> m_command_buffers;
};

class RenderGraphicsCommand final : public RenderCommand
{
public:
    explicit RenderGraphicsCommand(RenderDriver const* driver);
    ~RenderGraphicsCommand() override;

    void begin_pass() override;
    void end_pass() override;
    void submit(RenderCommandSubmitInfo const& info) override;
};

class RenderComputeCommand final : public RenderCommand
{
public:
    explicit RenderComputeCommand(RenderDriver const* driver);
    ~RenderComputeCommand() override;

    void begin_pass() override;
    void end_pass() override;
    void submit(RenderCommandSubmitInfo const& info) override;
};

AMAZING_NAMESPACE_END

#endif //RENDER_COMMAND_H
