//
// Created by AmazingBuff on 2025/4/23.
//

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUMemoryPool
{
public:
    GPUMemoryPool() : m_ref_device(nullptr), m_type(GPUMemoryPoolType::e_automatic) {}
    virtual ~GPUMemoryPool() = default;

protected:
    GPUDevice const* m_ref_device;
    GPUMemoryPoolType m_type;
};

AMAZING_NAMESPACE_END

#endif //MEMORY_POOL_H
