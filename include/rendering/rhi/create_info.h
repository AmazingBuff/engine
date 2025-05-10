//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef CREATE_INFO_H
#define CREATE_INFO_H

#include "structure.h"

AMAZING_NAMESPACE_BEGIN

static constexpr size_t Rendering_Hash = hash_str("Rendering", Amazing_Hash);


struct GPUInstanceCreateInfo
{
    GPUBackend backend;
    bool enable_debug_layer;
    bool enable_gpu_based_validation;
    // this parameter for extensions
    const void* next;
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
    Vector<GPUQueue const*> present_queues;
};

struct GPUCommandPoolCreateInfo
{
    String name;
};

struct GPUCommandBufferCreateInfo
{
    bool is_secondary;
};

struct GPUQueryPoolCreateInfo
{
    GPUQueryType query_type;
    uint32_t query_count;
};

struct GPURootSignaturePoolCreateInfo
{
    String name;
};

struct GPURootSignatureCreateInfo
{
    Vector<GPUShaderEntry> shaders;
    Vector<GPUStaticSampler> static_samplers;
    Vector<String> push_constant_names;
    GPURootSignaturePool* pool;
};

struct GPUDescriptorSetCreateInfo
{
    GPURootSignature const* root_signature;
    uint32_t set_index;
};

struct GPUGraphicsPipelineCreateInfo
{
    GPURootSignature* root_signature;
    GPUShaderEntry const* vertex_shader;
    GPUShaderEntry const* tessellation_control_shader;
    GPUShaderEntry const* tessellation_evaluation_shader;
    GPUShaderEntry const* geometry_shader;
    GPUShaderEntry const* fragment_shader;
    Vector<GPUVertexAttribute> const vertex_inputs;

    GPUBlendState const* blend_state;
    GPUDepthStencilState const* depth_stencil_state;
    GPURasterizerState const* rasterizer_state;

    GPUFormat const* color_format;
    GPUFormat depth_stencil_format;

    uint32_t render_target_count;
    GPUSampleCount sample_count;
    uint32_t sample_quality;

    GPUPrimitiveTopology primitive_topology;
};

struct GPUGraphicsPassCreateInfo
{
    String name;
    GPUSampleCount sample_count;
    Vector<GPUColorAttachment> color_attachments;
    GPUDepthStencilAttachment const* depth_stencil_attachment;
};


// resources
struct GPUMemoryPoolCreateInfo
{
    GPUMemoryPoolType type;
    GPUMemoryUsage usage;
    uint64_t block_size;
    uint32_t min_block_count;
    uint32_t max_block_count;
    uint64_t min_allocation_alignment;
};

struct GPUBufferCreateInfo
{
    String name;
    uint64_t size;
    struct
    {
        uint64_t first_element;
        uint32_t element_count;
        uint32_t element_stride;
    } data_array;
    GPUBuffer const* counter_buffer;
    GPUMemoryUsage usage;
    GPUFormat format;
    GPUResourceState state;
    GPUResourceType type;
    GPUBufferFlags flags;
};

struct GPUTextureCreateInfo
{
    String name;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t array_layers;
    uint32_t mip_levels;
    uint32_t sample_quality;
    GPUSampleCount sample_count;
    GPUFormat format;
    GPUResourceState state;
    GPUResourceType type;
    GPUTextureFlags flags;
    GPUClearColor clear_color;
};

struct GPUTextureViewCreateInfo
{
    String name;
    GPUTexture* texture;
    GPUFormat format;
    GPUTextureViewUsage usage;
    GPUTextureViewAspect aspect;
    GPUTextureType type;
    uint32_t base_array_layer;
    uint32_t array_layers;
    uint32_t base_mip_level;
    uint32_t mip_levels;
};

struct GPUSamplerCreateInfo
{
    GPUFilterType min_filter;
    GPUFilterType mag_filter;
    GPUMipMapMode mipmap_mode;
    GPUAddressMode address_u;
    GPUAddressMode address_v;
    GPUAddressMode address_w;
    GPUCompareMode compare_mode;
    float mip_lod_bias;
    float max_anisotropy;
};

struct GPUShaderLibraryCreateInfo
{
    String name;
    const uint32_t* code;
    uint32_t code_size;
    GPUShaderStage stage;
    bool reflection;
};


// process
struct GPUResourceBarrierInfo
{
    Vector<GPUBufferBarrier> buffer_barriers;
    Vector<GPUTextureBarrier> texture_barriers;
};

struct GPUQueueSubmitInfo
{
    Vector<GPUCommandBuffer const*> command_buffers;
    Vector<GPUSemaphore const*> wait_semaphores;
    Vector<GPUSemaphore*> signal_semaphores;
    GPUFence* signal_fence;
};

struct GPUQueuePresentInfo
{
    GPUSwapChain const* swap_chain;
    Vector<GPUSemaphore const*> wait_semaphores;
    uint8_t index;
};

struct GPUBufferToTextureTransferInfo
{
    GPUBuffer const* src_buffer;
    uint64_t src_buffer_offset;
    GPUTexture const* dst_texture;
    GPUTextureSubresource dst_texture_subresource;
};

AMAZING_NAMESPACE_END

#endif //CREATE_INFO_H
