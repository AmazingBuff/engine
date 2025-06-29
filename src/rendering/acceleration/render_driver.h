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
struct RenderGraphTexture;
struct RenderGraphPipeline;

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

    [[nodiscard]] RenderGeometry import_render_geometry(Scene const& scene) const;
    RenderGeometry import_render_geometry(const char* file_name) const;
    void destroy_render_geometry(RenderGeometry const& geometry) const;

    [[nodiscard]] RenderGraphPipeline create_pipeline(RenderGraphPipelineCreateInfo const& info) const;
    void destroy_pipeline(RenderGraphPipeline const& pipeline) const;

public:
    struct RenderDriverInfo
    {
        RenderBackend backend;
        uint32_t frame_count;
    } m_driver_info;
private:
    GPUInstance const* m_instance;
    GPUDevice const* m_device;
    GPUQueue const* m_graphics_queue;
    GPUQueue const* m_compute_queue;

    // todo: add external control to select sampler
    GPUSampler const* m_static_samplers[Reflect::MetaInfo<GPUFilterType>::enum_count() * Reflect::MetaInfo<GPUAddressMode>::enum_count()];

    friend class RenderAssetManager;
    friend class RenderGraphicsCommand;
    friend class RenderComputeCommand;
};

AMAZING_NAMESPACE_END

#endif //RENDER_DRIVER_H
