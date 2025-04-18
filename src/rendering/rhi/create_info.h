//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef CREATE_INFO_H
#define CREATE_INFO_H

#include "structure.h"

AMAZING_NAMESPACE_BEGIN

static constexpr size_t Rendering_Hash = hash_combine(Amazing_Hash, hash_str("Rendering"));


struct GPUInstanceCreateInfo
{
    GPUBackend backend;
    bool enable_debug_layer;
    bool enable_gpu_based_validation;
};

struct GPUDeviceCreateInfo
{
    bool disable_pipeline_cache;
    Vector<GPUQueueGroup> queue_groups;
};

struct GPUSwapChainCreateInfo
{
    uint32_t width;
    uint32_t height;
    uint32_t frame_count;
    GPUFormat format;
    bool enable_vsync;
    GPUSurface* surface;
    Vector<GPUQueue*> present_queues;
};

struct GPUCommandBufferCreateInfo
{

};

struct GPUQueryPoolCreateInfo
{
    GPUQueryType query_type;
    uint32_t query_count;
};

struct GPUTextureCreateInfo
{

};


AMAZING_NAMESPACE_END

#endif //CREATE_INFO_H
