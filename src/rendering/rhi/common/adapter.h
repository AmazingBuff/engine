//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef ADAPTER_H
#define ADAPTER_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUAdapter
{
public:
    GPUAdapter() : m_adapter_detail{}, m_ref_instance(nullptr) {}
    virtual ~GPUAdapter() = default;
    virtual void query_memory_usage(GPUDevice const* device, uint64_t* total, uint64_t* used) = 0;
public:
    struct GPUAdapterVendor
    {
        uint32_t device_id;
        uint32_t vendor_id;
        uint32_t driver_version;
        char gpu_name[GPU_Vendor_String_Length];
    };

    struct GPUFormatSupport
    {
        uint8_t shader_read : 1;
        uint8_t shader_write : 1;
        uint8_t render_target_write : 1;
        uint8_t depth_stencil_write : 1;
        uint8_t mipmap_write: 1;
    };

    struct GPUAdapterDetail
    {
        GPUAdapterVendor vendor;
        GPUFormatSupport format_support[GPU_Format_Count];
        GPUDynamicState dynamic_states;
        uint32_t uniform_buffer_alignment;
        uint32_t upload_buffer_texture_alignment;
        uint32_t upload_buffer_texture_row_alignment;
        uint32_t max_vertex_input_bindings;
        uint32_t wave_lane_count;
        uint64_t host_visible_vram_budget;

        bool support_host_visible_vram : 1;
        bool multidraw_indirect : 1;
        bool support_geom_shader : 1;
        bool support_tessellation : 1;
        bool support_mesh_shader : 1;
        bool is_uma : 1;
        bool is_virtual : 1;
        bool is_cpu : 1;
        bool support_tiled_buffer : 1;
        bool support_tiled_texture : 1;
        bool support_tiled_volume : 1;
        bool support_ray_tracing : 1;
        // RDNA2
        bool support_shading_rate : 1;
        bool support_shading_rate_mask : 1;
        bool support_shading_rate_sv : 1;
    };

    GPUAdapterDetail m_adapter_detail;
protected:
    GPUInstance const* m_ref_instance;
};


AMAZING_NAMESPACE_END

#endif //ADAPTER_H
