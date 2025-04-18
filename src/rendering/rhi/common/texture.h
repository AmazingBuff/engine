//
// Created by AmazingBuff on 2025/4/17.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUTexture
{
public:
    GPUTexture() : m_info(nullptr) {}
    virtual ~GPUTexture() = 0;

    virtual AResult initialize(GPUDevice const* device, GPUTextureCreateInfo const& info) = 0;
protected:
    struct GPUTextureInfo
    {
        uint64_t width;
        uint64_t height;
        uint64_t depth;
        uint32_t mip_levels;
        uint32_t array_size_minus_one;
        uint64_t size_in_bytes;
        GPUFormat format;
        GPUSampleCount sample_count;
        uint64_t unique_id;
        uint32_t aspect_mask;
        uint32_t node_index;
        uint8_t owns_image;
        uint8_t is_cube;
        uint8_t is_allocation_dedicated;
        uint8_t is_restrict_dedicated;
        uint8_t is_aliasing;
        uint8_t is_tiled;
        uint8_t is_imported;
        uint8_t can_alias;
        uint8_t can_export;
    };
    GPUTextureInfo* m_info;
};

AMAZING_NAMESPACE_END

#endif //TEXTURE_H
