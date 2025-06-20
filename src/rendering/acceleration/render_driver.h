//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_DRIVER_H
#define RENDER_DRIVER_H

#include "rendering/render_type.h"
#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

struct RenderDriverCreateInfo
{
    RenderBackend backend;
    uint32_t frame_count;
};

class RenderDriver
{
public:
    explicit RenderDriver(RenderDriverCreateInfo const& info);
    ~RenderDriver();

private:
    struct RenderDriverInfo
    {
        RenderBackend backend;
        uint32_t frame_count;
    } m_driver_info;

    GPUInstance* m_instance;
    GPUDevice* m_device;
    GPUQueue* m_graphics_queue;
    GPUQueue* m_compute_queue;
    Vector<GPUCommandPool*> m_command_pools;
    Vector<GPUCommandBuffer*> m_command_buffers;
    Vector<GPUCommandPool*> m_compute_pools;
    Vector<GPUCommandBuffer*> m_compute_buffers;

    friend class RenderAssetManager;
};

AMAZING_NAMESPACE_END

#endif //RENDER_DRIVER_H
