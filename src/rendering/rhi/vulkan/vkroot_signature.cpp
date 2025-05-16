//
// Created by AmazingBuff on 2025/5/12.
//

#include "vkroot_signature.h"
#include "vkdevice.h"
#include "internal/vkdescriptor_pool.h"
#include "resources/vksampler.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"
#include "rendering/rhi/common/root_signature_pool.h"

AMAZING_NAMESPACE_BEGIN

VKRootSignature::VKRootSignature(GPUDevice const* device, GPURootSignatureCreateInfo const& info)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);

    initialize(info);
    if (info.pool)
    {
        VKRootSignature const* pool_root_signature = static_cast<VKRootSignature const*>(info.pool->find(this, info));
        if (pool_root_signature)
        {
            m_pipeline_layout = pool_root_signature->m_pipeline_layout;
            m_push_constant_ranges = pool_root_signature->m_push_constant_ranges;
            m_descriptor_layouts = pool_root_signature->m_descriptor_layouts;
            m_pool = info.pool;
            m_ref_device = device;

            return;
        }
    }

    // obtain num
    Map<uint32_t, uint32_t> set_count_map;
    for (GPUParameterTable const& table : m_tables)
        set_count_map[table.set_index] += table.resources.size();
    for (GPUShaderResource const& sampler : m_static_samplers)
        set_count_map[sampler.set] += 1;

    // binding
    for (auto const& [set, count] : set_count_map)
    {
        VkDescriptorSetLayoutBinding* bindings = static_cast<VkDescriptorSetLayoutBinding*>(alloca(count * sizeof(VkDescriptorSetLayoutBinding)));
        uint32_t binding_count = 0;
        for (GPUParameterTable const& table : m_tables)
        {
            if (table.set_index == set)
            {
                for (GPUShaderResource const& resource : table.resources)
                {
                    bindings[binding_count].binding = resource.binding;
                    bindings[binding_count].stageFlags = transfer_shader_stage(resource.stage);
                    bindings[binding_count].descriptorType = transfer_resource_type(resource.resource_type);
                    bindings[binding_count].descriptorCount = resource.array_count;
                    binding_count++;
                }
                break;
            }
        }
        for (size_t i = 0; i < info.static_samplers.size(); i++)
        {
            if (m_static_samplers[i].set == set)
            {
                bindings[binding_count].binding = m_static_samplers[i].binding;
                bindings[binding_count].stageFlags = transfer_shader_stage(m_static_samplers[i].stage);
                bindings[binding_count].descriptorType = transfer_resource_type(m_static_samplers[i].resource_type);
                bindings[binding_count].descriptorCount = m_static_samplers[i].array_count;
                bindings[binding_count].pImmutableSamplers = &static_cast<VKSampler const*>(info.static_samplers[i].sampler)->m_sampler;
                binding_count++;
            }
        }
        VkDescriptorSetLayoutCreateInfo set_layout_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = binding_count,
            .pBindings = bindings,
        };

        VulkanDescriptorLayout descriptor_layout;
        VK_CHECK_RESULT(vk_device->m_device_table.vkCreateDescriptorSetLayout(vk_device->m_device, &set_layout_info, VK_Allocation_Callbacks_Ptr, &descriptor_layout.set_layout));
        descriptor_layout.set = vk_device->m_descriptor_pool->consume_descriptor_set(&descriptor_layout.set_layout, 1);
        m_descriptor_layouts.emplace(set, descriptor_layout);
    }

    // push constant
    m_push_constant_ranges = Allocator<VkPushConstantRange>::allocate(m_push_constant_count);
    for (uint32_t i = 0; i < m_push_constant_count; i++)
    {
        m_push_constant_ranges[i].stageFlags = transfer_shader_stage(m_push_constants[i].stage);
        m_push_constant_ranges[i].offset = m_push_constants[i].offset;
        m_push_constant_ranges[i].size = m_push_constants[i].size;
    }

    // layout set
    VkDescriptorSetLayout* layouts = m_descriptor_layouts.empty() ? nullptr : static_cast<VkDescriptorSetLayout*>(alloca(sizeof(VkDescriptorSetLayout) * m_descriptor_layouts.size()));
    uint32_t set_layout_count = 0;
    for (auto const& [set, layout]: m_descriptor_layouts)
    {
        layouts[set_layout_count] = layout.set_layout;
        set_layout_count++;
    }
    // pipeline layout
    VkPipelineLayoutCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = set_layout_count,
        .pSetLayouts = layouts,
        .pushConstantRangeCount = m_push_constant_count,
        .pPushConstantRanges = m_push_constant_ranges,
    };
    VK_CHECK_RESULT(vk_device->m_device_table.vkCreatePipelineLayout(vk_device->m_device, &pipeline_info, VK_Allocation_Callbacks_Ptr, &m_pipeline_layout));

    // update template
    for (GPUParameterTable const& table : m_tables)
    {
        uint32_t update_entry_count = table.resources.size();
        VkDescriptorUpdateTemplateEntry* entries = static_cast<VkDescriptorUpdateTemplateEntry*>(alloca(sizeof(VkDescriptorUpdateTemplateEntry) * update_entry_count));
        for (size_t i = 0; i < update_entry_count; i++)
        {
            entries[i].descriptorCount = table.resources[i].array_count;
            entries[i].descriptorType = transfer_resource_type(table.resources[i].resource_type);
            entries[i].dstBinding = table.resources[i].binding;
            entries[i].dstArrayElement = 0;
            entries[i].stride = sizeof(VulkanDescriptorUpdateData);
            entries[i].offset = entries[i].dstBinding * entries[i].stride;
        }
        if (update_entry_count > 0)
        {
            VulkanDescriptorLayout& layout = m_descriptor_layouts[table.set_index];
            layout.update_entry_count = update_entry_count;

            VkDescriptorUpdateTemplateCreateInfo template_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO,
                .descriptorUpdateEntryCount = update_entry_count,
                .pDescriptorUpdateEntries = entries,
                .templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET_KHR,
                .descriptorSetLayout = layout.set_layout,
                .pipelineBindPoint = Pipeline_Bind_Point_Map[to_underlying(m_pipeline_type)],
                .pipelineLayout = m_pipeline_layout,
                .set = table.set_index,
            };
            VK_CHECK_RESULT(vk_device->m_device_table.vkCreateDescriptorUpdateTemplate(vk_device->m_device, &template_info, VK_Allocation_Callbacks_Ptr, &layout.update_template));
        }
    }

    if (info.pool)
    {
        VKRootSignature* pool_root_signature = static_cast<VKRootSignature*>(info.pool->insert(this, info));
        if (pool_root_signature)
        {
            m_pipeline_layout = pool_root_signature->m_pipeline_layout;
            m_push_constant_ranges = pool_root_signature->m_push_constant_ranges;
            m_descriptor_layouts = pool_root_signature->m_descriptor_layouts;
            m_pool = info.pool;
        }
    }
    m_ref_device = device;
}

VKRootSignature::~VKRootSignature()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

    if (m_pool)
        m_pool->remove(this);

    if (m_push_constant_ranges)
        Allocator<VkPushConstantRange>::deallocate(m_push_constant_ranges);
    m_push_constant_ranges = nullptr;

    for (auto const& [set, layout]: m_descriptor_layouts)
    {
        vk_device->m_device_table.vkDestroyDescriptorSetLayout(vk_device->m_device, layout.set_layout, VK_Allocation_Callbacks_Ptr);
        vk_device->m_device_table.vkDestroyDescriptorUpdateTemplate(vk_device->m_device, layout.update_template, VK_Allocation_Callbacks_Ptr);
    }

    vk_device->m_device_table.vkDestroyPipelineLayout(vk_device->m_device, m_pipeline_layout, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END