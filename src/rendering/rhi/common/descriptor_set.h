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
    GPUDescriptorSet() : m_ref_device(nullptr), m_ref_root_signature(nullptr), m_set_index(0) {}
    virtual ~GPUDescriptorSet() = default;

    virtual void update(Vector<GPUDescriptorData> const& descriptor_data) = 0;
protected:
    GPUDevice const* m_ref_device;
    GPURootSignature const* m_ref_root_signature;
    uint32_t m_set_index;
};

AMAZING_NAMESPACE_END

#endif //DESCRIPTOR_SET_H
