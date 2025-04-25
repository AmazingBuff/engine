//
// Created by AmazingBuff on 2025/4/24.
//

#ifndef DX12DESCRIPTOR_SET_H
#define DX12DESCRIPTOR_SET_H

#include "rendering/rhi/common/descriptor_set.h"

AMAZING_NAMESPACE_BEGIN

class DX12DescriptorSet final : public GPUDescriptorSet
{
public:
    DX12DescriptorSet();
    ~DX12DescriptorSet();

    AResult initialize(GPUDevice const* device, GPUDescriptorSetCreateInfo const& info) override;
    void update(Vector<GPUDescriptorData> const& descriptor_data) override;
private:
    uint64_t m_cbv_srv_uav_handle;
    uint32_t m_cbv_srv_uav_stride;
    uint64_t m_sampler_handle;
    uint32_t m_sampler_stride;
};

AMAZING_NAMESPACE_END

#endif //DX12DESCRIPTOR_SET_H
