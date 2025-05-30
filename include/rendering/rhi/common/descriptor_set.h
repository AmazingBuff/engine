//
// Created by AmazingBuff on 2025/4/24.
//

#ifndef DESCRIPTOR_SET_H
#define DESCRIPTOR_SET_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUDescriptorSet
{
public:
    GPUDescriptorSet() : m_ref_root_signature(nullptr), m_set_index(0) {}
    virtual ~GPUDescriptorSet() = default;

    virtual void update(GPUDescriptorData const* descriptor_data, uint32_t descriptor_data_count) = 0;
protected:
    GPURootSignature const* m_ref_root_signature;
    uint32_t m_set_index;
};

AMAZING_NAMESPACE_END

#endif //DESCRIPTOR_SET_H
