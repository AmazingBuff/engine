//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef BUFFER_H
#define BUFFER_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUBuffer
{
protected:
    struct GPUBufferInfo
    {
        uint64_t size;
        void* mapped_data;
        GPUResourceType type;
        GPUMemoryUsage memory_usage;
        GPUBufferFlag flags;
        GPUResourceState state;
    };
public:
    GPUBuffer() : m_ref_device(nullptr), m_info(nullptr) {}
    virtual ~GPUBuffer() = default;

    virtual void map(size_t offset, size_t size, const void* data = nullptr) const = 0;
    virtual void unmap() const = 0;

    GPUBufferInfo const* descriptor() const { return m_info; }
protected:
    GPUDevice const* m_ref_device;
    GPUBufferInfo* m_info;
};

AMAZING_NAMESPACE_END

#endif //BUFFER_H
