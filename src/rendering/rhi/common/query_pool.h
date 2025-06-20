//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef QUERY_POOL_H
#define QUERY_POOL_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUQueryPool
{
public:
    GPUQueryPool() : m_ref_device(nullptr), m_query_type(GPUQueryType::e_timestamp) {}
    virtual ~GPUQueryPool() = default;
protected:
    GPUDevice const* m_ref_device;
    GPUQueryType m_query_type;
};

AMAZING_NAMESPACE_END

#endif //QUERY_POOL_H
