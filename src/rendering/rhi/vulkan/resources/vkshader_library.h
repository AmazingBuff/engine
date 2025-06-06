//
// Created by AmazingBuff on 2025/5/9.
//

#ifndef VKSHADER_LIBRARY_H
#define VKSHADER_LIBRARY_H

#include "rendering/rhi/common/shader_library.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN


class VKShaderLibrary final : public GPUShaderLibrary
{
public:
    VKShaderLibrary(GPUDevice const* device, GPUShaderLibraryCreateInfo const& info);
    ~VKShaderLibrary() override;
private:
    VkShaderModule m_shader_module;

    friend class VKGraphicsPipeline;
    friend class VKComputePipeline;
};


AMAZING_NAMESPACE_END

#endif //VKSHADER_LIBRARY_H
