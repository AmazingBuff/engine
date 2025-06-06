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
    explicit DX12DescriptorSet(GPUDescriptorSetCreateInfo const& info);
    ~DX12DescriptorSet() override;

    void update(GPUDescriptorData const* descriptor_data, uint32_t descriptor_data_count) override;
private:
    uint64_t m_cbv_srv_uav_handle;
    uint32_t m_cbv_srv_uav_stride;
    uint64_t m_sampler_handle;
    uint32_t m_sampler_stride;

    friend class DX12GraphicsPassEncoder;
    friend class DX12ComputePassEncoder;
};

AMAZING_NAMESPACE_END

#endif //DX12DESCRIPTOR_SET_H
