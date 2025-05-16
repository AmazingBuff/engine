//
// Created by AmazingBuff on 2025/5/12.
//

#ifndef VKGRAPHICS_PIPELINE_H
#define VKGRAPHICS_PIPELINE_H

#include "vk.h"
#include "rendering/rhi/common/graphics_pipeline.h"

AMAZING_NAMESPACE_BEGIN

class VKGraphicsPipeline final : public GPUGraphicsPipeline
{
public:
    explicit VKGraphicsPipeline(GPUGraphicsPipelineCreateInfo const& info);
    ~VKGraphicsPipeline() override;
private:
    VkPipeline m_pipeline;

    friend class VKGraphicsPassEncoder;
};

AMAZING_NAMESPACE_END

#endif //VKGRAPHICS_PIPELINE_H
