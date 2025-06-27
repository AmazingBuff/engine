//
// Created by AmazingBuff on 2025/6/27.
//

#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class RenderDriver;

class RenderCommand
{
public:
    explicit RenderCommand(RenderDriver const* driver);
    ~RenderCommand();

private:
    uint32_t m_frame_index;
    uint32_t m_frame_count;

    GPUQueue* m_graphics_queue;
    GPUQueue* m_compute_queue;
    Vector<GPUCommandPool*> m_command_pools;
    Vector<GPUCommandBuffer*> m_command_buffers;
    Vector<GPUCommandPool*> m_compute_pools;
    Vector<GPUCommandBuffer*> m_compute_buffers;
};

AMAZING_NAMESPACE_END

#endif //RENDER_COMMAND_H
