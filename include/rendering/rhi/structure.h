//
// Created by AmazingBuff on 2025/4/15.
//

#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "enumeration.h"

AMAZING_NAMESPACE_BEGIN

static constexpr uint32_t GPU_Max_Render_Target = 8;
static constexpr uint32_t GPU_Max_Push_Constant_Size = 128;

class GPUSurface;
class GPUInstance;
class GPUAdapter;
class GPUDevice;
class GPUFence;
class GPUSemaphore;
class GPUSwapChain;
class GPUQueue;
class GPUCommandPool;
class GPUCommandBuffer;
class GPUQueryPool;
class GPURootSignature;
class GPURootSignaturePool;
class GPUDescriptorSet;
class GPUGraphicsPipeline;
class GPUGraphicsPassEncoder;
class GPUComputePipeline;
class GPUComputePassEncoder;

class GPUMemoryPool;
class GPUBuffer;
class GPUTexture;
class GPUTextureView;
class GPUSampler;
class GPUShaderLibrary;


struct GPUQueueGroup
{
    GPUQueueType    queue_type;
    uint32_t        queue_count;
};

struct GPUQueryInfo
{
    GPUShaderStage  shader_stage;
    uint32_t        index;
};

struct GPUClearColor
{
    union
    {
        struct
        {
            float r;
            float g;
            float b;
            float a;
        } color;
        struct
        {
            float depth;
            uint32_t stencil;
        } depth_stencil;
    };
};

struct GPUShaderResource
{
    String name;
    GPUResourceType resource_type;
    GPUTextureType texture_type;
    GPUShaderStage stage;
    uint64_t name_hash;
    uint32_t set;
    uint32_t binding;
    uint32_t size;
    uint32_t array_count;
    uint32_t offset;
};

struct GPUVertexInput
{
    String name;
    String semantics;
    GPUFormat format;
};

struct GPUShaderReflection
{
    String entry_name;
    GPUShaderStage stage;
    Vector<GPUVertexInput> vertex_inputs;
    Vector<GPUShaderResource> shader_resources;
    uint32_t thread_group_sizes[3];
};

struct GPUConstantSpecialization
{
    uint32_t constant_id;
    union
    {
        uint64_t    u;
        int64_t     i;
        double      d;
    };
};

struct GPUShaderEntry
{
    GPUShaderLibrary const* library;
    String entry;
    GPUShaderStage stage;
    Vector<GPUConstantSpecialization> constants;
};

struct GPUParameterTable
{
    Vector<GPUShaderResource> resources;
    uint32_t set_index;
};

struct GPUStaticSampler
{
    String name;
    GPUSampler const* sampler;
};

struct GPUDescriptorData
{
    // via resource name
    String name;
    // via set and binding
    GPUResourceType resource_type;
    uint32_t binding;


    // resource
    union
    {
        GPUSampler const* const* samplers;
        GPUTextureView const* const* textures;
        GPUBuffer const* const* buffers;
    };

    uint32_t array_count;
};

struct GPUVertexAttribute
{
    uint32_t array_size;
    GPUFormat format;
    uint32_t slot;          // for d3d12 is slot, while vulkan is binding
    String semantic_name;   // d3d12 use semantic while vulkan use location
    uint32_t location;
    uint32_t offset;
    uint32_t size;          // every attribute size
    GPUVertexInputRate rate;
};

struct GPUBlendState
{
    GPUBlendConstant src_factors[GPU_Max_Render_Target];
    GPUBlendConstant dst_factors[GPU_Max_Render_Target];
    GPUBlendMode blend_modes[GPU_Max_Render_Target];
    GPUBlendConstant alpha_src_factors[GPU_Max_Render_Target];
    GPUBlendConstant alpha_dst_factors[GPU_Max_Render_Target];
    GPUBlendMode alpha_blend_modes[GPU_Max_Render_Target];
    uint8_t write_masks[GPU_Max_Render_Target];

    bool alpha_to_coverage;
    bool independent_blend;
};

struct GPUDepthStencilState
{
    bool depth_test;
    bool depth_write;
    GPUCompareMode depth_compare;
    bool stencil_test;
    uint8_t stencil_read_mask;
    uint8_t stencil_write_mask;

    struct GPUStencilState
    {
        GPUCompareMode stencil_compare;
        GPUStencilOp stencil_fail;
        GPUStencilOp stencil_pass;
        GPUStencilOp depth_fail;
    };

    GPUStencilState front;
    GPUStencilState back;
};

struct GPURasterizerState
{
    GPUCullMode cull_mode;
    int32_t depth_bias;
    float slope_scaled_depth_bias;
    GPUFillMode fill_mode;
    GPUFrontFace front_face;
    bool enable_multisample;
    bool enable_depth_clamp;
};

struct GPUBufferBarrier
{
    GPUBuffer const* buffer;
    GPUResourceState src_state;
    GPUResourceState dst_state;
    uint8_t queue_acquire;
    uint8_t queue_release;
    GPUQueueType queue_type;
};

struct GPUTextureBarrier
{
    GPUTexture const* texture;
    GPUResourceState dst_state;
    uint8_t queue_acquire;
    uint8_t queue_release;
    GPUQueueType queue_type;

    // make following parameters add to barrier
    uint8_t subresource_barrier;
    uint32_t mip_level;
    uint32_t array_layer;
};

struct GPUColorAttachment
{
    GPUTextureView const* texture_view;
    GPUTextureView const* resolve_view;
    GPULoadAction load;
    GPUStoreAction store;
    GPUClearColor clear_color;
};

struct GPUDepthStencilAttachment
{
    GPUTextureView const* texture_view;
    GPULoadAction depth_load;
    GPUStoreAction depth_store;
    GPULoadAction stencil_load;
    GPUStoreAction stencil_store;
    GPUClearColor clear_color;
    uint8_t depth_write;
    uint8_t stencil_write;
};

struct GPUBufferBinding
{
    GPUBuffer const* buffer;
    uint32_t stride;
    uint32_t offset;
};

struct GPUTextureSubresource
{
    GPUTextureViewAspect aspect;
    uint32_t mip_level;
    uint32_t base_array_layer;
    uint32_t array_layers;
};

AMAZING_NAMESPACE_END

#endif //STRUCTURE_H
