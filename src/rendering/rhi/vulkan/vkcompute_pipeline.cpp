//
// Created by AmazingBuff on 2025/5/26.
//

#include "vkcompute_pipeline.h"
#include "vkroot_signature.h"
#include "vkdevice.h"
#include "resources/vkshader_library.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKComputePipeline::VKComputePipeline(GPUComputePipelineCreateInfo const& info)
{
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(info.root_signature);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_root_signature->m_ref_device);

    VKShaderLibrary const* compute_shader = static_cast<VKShaderLibrary const*>(info.compute_shader->library);
    VkPipelineShaderStageCreateInfo compute_shader_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = compute_shader->m_shader_module,
        .pName = info.compute_shader->entry.c_str(),
        .pSpecializationInfo = nullptr
    };

    VkComputePipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = compute_shader_info,
        .layout = vk_root_signature->m_pipeline_layout,
        .basePipelineHandle = nullptr,
        .basePipelineIndex = 0
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateComputePipelines(vk_device->m_device, vk_device->m_pipeline_cache, 1, &pipeline_info, VK_Allocation_Callbacks_Ptr, &m_pipeline));

    m_ref_root_signature = vk_root_signature;
}

VKComputePipeline::~VKComputePipeline()
{
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(m_ref_root_signature);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_root_signature->m_ref_device);

    vk_device->m_device_table.vkDestroyPipeline(vk_device->m_device, m_pipeline, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END