//
// Created by AmazingBuff on 2025/5/12.
//

#include "vkdescriptor_set.h"
#include "vkdevice.h"
#include "vkroot_signature.h"
#include "internal/vkdescriptor_pool.h"
#include "resources/vktexture_view.h"
#include "resources/vkbuffer.h"
#include "resources/vksampler.h"

AMAZING_NAMESPACE_BEGIN

VKDescriptorSet::VKDescriptorSet(GPUDescriptorSetCreateInfo const& info)
{
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(info.root_signature);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_root_signature->m_ref_device);

    GPUParameterTable const* table = nullptr;
    for (GPUParameterTable const& root_table : vk_root_signature->m_tables)
    {
        if (root_table.set_index == info.set_index)
        {
            m_set_index = info.set_index;
            table = &root_table;
            break;
        }
    }

    RENDERING_ASSERT(table != nullptr, "failed to find valid set, the set is {}", info.set_index);

    VKRootSignature::VulkanDescriptorLayout const& layout = vk_root_signature->m_descriptor_layouts[m_set_index];
    m_descriptor_set = vk_device->m_descriptor_pool->consume_descriptor_set(&layout.set_layout, 1);

    uint32_t data_count = 0;
    for (GPUShaderResource const& resource : table->resources)
        data_count += resource.array_count;
    m_update_data = Allocator<VulkanDescriptorUpdateData>::allocate(data_count);

    m_ref_root_signature = info.root_signature;
}

VKDescriptorSet::~VKDescriptorSet()
{
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(m_ref_root_signature);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_root_signature->m_ref_device);

    vk_device->m_descriptor_pool->return_descriptor_set(&m_descriptor_set, 1);
    Allocator<VulkanDescriptorUpdateData>::deallocate(m_update_data);
}

void VKDescriptorSet::update(GPUDescriptorData const* descriptor_data, uint32_t descriptor_data_count)
{
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(m_ref_root_signature);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_root_signature->m_ref_device);

    GPUParameterTable const* table = nullptr;
    for (GPUParameterTable const& root_table : vk_root_signature->m_tables)
    {
        if (root_table.set_index == m_set_index)
        {
            table = &root_table;
            break;
        }
    }

    bool need_update = false;
    for (uint32_t i = 0; i < descriptor_data_count; ++i)
    {
        GPUDescriptorData const& data = descriptor_data[i];
        GPUShaderResource const* resource = nullptr;
        if (!data.name.empty())
        {
            size_t name_hash = hash_str(data.name.c_str(), data.name.size(), VK_Hash);
            for (GPUShaderResource const& shader_resource : table->resources)
            {
                if (shader_resource.name_hash == name_hash)
                {
                    resource = &shader_resource;
                    break;
                }
            }
        }
        else
        {
            for (GPUShaderResource const& shader_resource : table->resources)
            {
                if (shader_resource.binding == data.binding)
                {
                    resource = &shader_resource;
                    break;
                }
            }
        }

        if (resource)
        {
            switch (resource->resource_type)
            {
            case GPUResourceType::e_rw_texture:
            case GPUResourceType::e_texture:
            {
                for (uint32_t j = 0; j < data.array_count; ++j)
                {
                    VKTextureView const* texture_view = static_cast<VKTextureView const*>(data.textures[j]);
                    VulkanDescriptorUpdateData& update_data = m_update_data[resource->binding + j];
                    if (resource->resource_type == GPUResourceType::e_rw_texture)
                    {
                        update_data.image_info.imageView = texture_view->m_uav_view;
                        update_data.image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    }
                    else
                    {
                        update_data.image_info.imageView = texture_view->m_srv_view;
                        update_data.image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    }
                    update_data.image_info.sampler = nullptr;
                }
                break;
            }
            case GPUResourceType::e_sampler:
            {
                for (uint32_t j = 0; j < data.array_count; ++j)
                {
                    VKSampler const* sampler = static_cast<VKSampler const*>(data.samplers[j]);
                    VulkanDescriptorUpdateData& update_data = m_update_data[resource->binding + j];
                    update_data.image_info.sampler = sampler->m_sampler;
                }
                break;
            }
            case GPUResourceType::e_uniform_buffer:
            case GPUResourceType::e_buffer:
            case GPUResourceType::e_buffer_raw:
            case GPUResourceType::e_rw_buffer:
            case GPUResourceType::e_rw_buffer_raw:
            {
                for (uint32_t j = 0; j < data.array_count; ++j)
                {
                    VKBuffer const* buffer = static_cast<VKBuffer const*>(data.buffers[j]);
                    VulkanDescriptorUpdateData& update_data = m_update_data[resource->binding + j];
                    update_data.buffer_info.buffer = buffer->m_buffer;
                    update_data.buffer_info.offset = 0;
                    update_data.buffer_info.range = buffer->m_info->size;
                }
                break;
            }
            default:
                RENDERING_LOG_ERROR("unsupported resource type {}", to_underlying(resource->resource_type));
                break;
            }
            need_update = true;
        }
    }

    if (need_update)
        vk_device->m_device_table.vkUpdateDescriptorSetWithTemplate(vk_device->m_device, m_descriptor_set, vk_root_signature->m_descriptor_layouts[m_set_index].update_template, m_update_data);
}


AMAZING_NAMESPACE_END