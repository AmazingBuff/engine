//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_DRIVER_H
#define RENDER_DRIVER_H

#include "rendering/render_type.h"
#include "rendering/rhi/create_info.h"
#include <refl/refl.h>

AMAZING_NAMESPACE_BEGIN

struct Scene;
struct RenderGeometry;
struct RenderGraphResource;
struct RenderGraphPipeline;
class RenderGraphicsCommand;

struct RenderDriverCreateInfo
{
    RenderBackend backend;
    uint32_t frame_count;

    void* window_handle;
    uint32_t frame_width;
    uint32_t frame_height;
    RenderFormat frame_format;
};

class RenderDriver
{
public:
    explicit RenderDriver(RenderDriverCreateInfo const& info);
    ~RenderDriver();

    NODISCARD RenderGeometry import_render_geometry(Scene const& scene) const;
    RenderGeometry import_render_geometry(const char* file_name) const;
    void destroy_render_geometry(RenderGeometry const& geometry) const;

    NODISCARD RenderGraphPipeline create_pipeline(RenderGraphPipelineCreateInfo const& info) const;
    void destroy_pipeline(RenderGraphPipeline const& pipeline) const;

    NODISCARD RenderGraphResource create_image(RenderGraphImageCreateInfo const& info) const;
    void destroy_resource(RenderGraphResource const& resource) const;

    void copy_to_swap_chain(RenderGraphResource const& resource, RenderGraphicsCommand& command) const;
    void present() const;
public:
    struct RenderDriverInfo
    {
        RenderBackend backend;
        uint32_t frame_count;
    } m_driver_info;
private:
    GPUInstance* m_instance;
    GPUDevice* m_device;
    GPUQueue* m_graphics_queue;
    GPUQueue* m_compute_queue;
    Vector<GPUFence*> m_fences;

    // present
    struct RenderPresentContext
    {
        GPUSurface* surface;
        GPUSwapChain* swap_chain;
        GPUSemaphore* image_semaphore;
        GPUSemaphore* present_semaphore;
        uint32_t swap_chain_image_index;
    };
    RenderPresentContext* m_present_context;


    // todo: add external control to select sampler
    GPUSampler* m_static_samplers[Reflect::MetaInfo<GPUFilterType>::enum_count() * Reflect::MetaInfo<GPUAddressMode>::enum_count()];

    friend class RenderGraphicsCommand;
    friend class RenderComputeCommand;
    friend class DrawRenderScene;
};

AMAZING_NAMESPACE_END

#endif //RENDER_DRIVER_H
