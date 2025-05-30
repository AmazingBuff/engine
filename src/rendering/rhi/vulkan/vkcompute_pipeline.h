//
// Created by AmazingBuff on 2025/5/26.
//

#ifndef VKCOMPUTE_PIPELINE_H
#define VKCOMPUTE_PIPELINE_H

#include "vk.h"
#include "rendering/rhi/common/compute_pipeline.h"

AMAZING_NAMESPACE_BEGIN

class VKComputePipeline final : public GPUComputePipeline
{
public:
    explicit VKComputePipeline(GPUComputePipelineCreateInfo const& info);
    ~VKComputePipeline() override;

private:
    VkPipeline m_pipeline;

    friend class VKComputePassEncoder;
};

AMAZING_NAMESPACE_END

#endif //VKCOMPUTE_PIPELINE_H
