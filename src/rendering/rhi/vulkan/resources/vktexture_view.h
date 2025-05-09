//
// Created by AmazingBuff on 2025/5/8.
//

#ifndef VKTEXTURE_VIEW_H
#define VKTEXTURE_VIEW_H

#include "rendering/rhi/common/texture_view.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKTextureView final : public GPUTextureView
{
public:
    VKTextureView(GPUDevice const* device, GPUTextureViewCreateInfo const& info);
    ~VKTextureView() override;
private:
    VkImageView m_rtv_dsv_view;
    VkImageView m_srv_view;
    VkImageView m_uav_view;
};

AMAZING_NAMESPACE_END

#endif //VKTEXTURE_VIEW_H
