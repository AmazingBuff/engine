//
// Created by AmazingBuff on 2025/5/14.
//

#ifndef VKQUERY_POOL_H
#define VKQUERY_POOL_H

#include "vk.h"
#include "rendering/rhi/common/query_pool.h"

AMAZING_NAMESPACE_BEGIN

class VKQueryPool final : public GPUQueryPool
{
public:
    VKQueryPool(GPUDevice const* device, GPUQueryPoolCreateInfo const& info);
    ~VKQueryPool() override;
private:
    VkQueryPool m_pool;

    friend class VKCommandBuffer;
};

AMAZING_NAMESPACE_END

#endif //VKQUERY_POOL_H
