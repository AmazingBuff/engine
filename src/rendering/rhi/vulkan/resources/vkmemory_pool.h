//
// Created by AmazingBuff on 2025/5/9.
//

#ifndef VKMEMORY_POOL_H
#define VKMEMORY_POOL_H

#include "rendering/rhi/common/memory_pool.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKMemoryPool final : public GPUMemoryPool
{
public:
    VKMemoryPool(GPUDevice const* device, GPUMemoryPoolCreateInfo const& info);
    ~VKMemoryPool() override;
private:
    VmaPool m_pool;
};

AMAZING_NAMESPACE_END

#endif //VKMEMORY_POOL_H
