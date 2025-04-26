//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef BUFFER_H
#define BUFFER_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUBuffer
{
public:
    GPUBuffer() : m_ref_device(nullptr), m_info(nullptr) {}
    virtual ~GPUBuffer() = default;

    virtual AResult initialize(GPUDevice const* device, GPUBufferCreateInfo const& info) = 0;
    virtual void map(size_t offset, size_t size) = 0;
    virtual void unmap() = 0;
protected:
    struct GPUBufferInfo
    {
        size_t size;
        void* mapped_data;
        GPUResourceType type;
    };

    GPUDevice const* m_ref_device;
    GPUBufferInfo* m_info;
};

AMAZING_NAMESPACE_END

#endif //BUFFER_H
