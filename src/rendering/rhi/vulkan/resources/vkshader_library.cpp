//
// Created by AmazingBuff on 2025/5/9.
//

#include "vkshader_library.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"
#include <spirv_cross.hpp>

AMAZING_NAMESPACE_BEGIN

static constexpr GPUTextureType Texture_Type_Map[] =
{
    GPUTextureType::e_1d,
    GPUTextureType::e_2d,
    GPUTextureType::e_3d,
    GPUTextureType::e_cube,
    GPUTextureType::e_undefined,
    GPUTextureType::e_undefined,
    GPUTextureType::e_undefined,
};

static constexpr GPUTextureType Texture_Array_Type_Map[] =
{
    GPUTextureType::e_1d_array,
    GPUTextureType::e_2d_array,
    GPUTextureType::e_undefined,
    GPUTextureType::e_cube_array,
    GPUTextureType::e_undefined,
    GPUTextureType::e_undefined,
    GPUTextureType::e_undefined,
};

static GPUShaderStage transfer_execution_model(spv::ExecutionModel model)
{
    switch (model)
    {
    case spv::ExecutionModel::ExecutionModelVertex:
        return GPUShaderStageFlag::e_vertex;
    case spv::ExecutionModel::ExecutionModelTessellationControl:
        return GPUShaderStageFlag::e_tessellation_control;
    case spv::ExecutionModel::ExecutionModelTessellationEvaluation:
        return GPUShaderStageFlag::e_tessellation_evaluation;
    case spv::ExecutionModel::ExecutionModelGeometry:
        return GPUShaderStageFlag::e_geometry;
    case spv::ExecutionModel::ExecutionModelFragment:
        return GPUShaderStageFlag::e_fragment;
    case spv::ExecutionModel::ExecutionModelGLCompute:
        return GPUShaderStageFlag::e_compute;
    default:
        return GPUShaderStageFlag::e_undefined;
    }
}

VKShaderLibrary::VKShaderLibrary(GPUDevice const* device, GPUShaderLibraryCreateInfo const& info) : m_shader_module(nullptr)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);

    VkShaderModuleCreateInfo shader_info{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = info.code_size,
        .pCode = info.code,
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateShaderModule(vk_device->m_device, &shader_info, VK_Allocation_Callbacks_Ptr, &m_shader_module));

    spirv_cross::Compiler compiler(info.code, info.code_size / 4);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    spirv_cross::SmallVector<spirv_cross::EntryPoint> entry_points = compiler.get_entry_points_and_stages();
    m_shader_reflections.resize(entry_points.size());
    for (size_t i = 0; i < entry_points.size(); ++i)
    {
        const spirv_cross::EntryPoint& entry_point = entry_points[i];
        compiler.set_entry_point(entry_point.name, entry_point.execution_model);

        GPUShaderReflection& reflection = m_shader_reflections[i];
        reflection.entry_name = entry_point.name.c_str();
        reflection.stage = transfer_execution_model(entry_point.execution_model);
        if (entry_point.execution_model == spv::ExecutionModel::ExecutionModelGLCompute)
        {
            const spirv_cross::SPIREntryPoint& entry = compiler.get_entry_point(entry_point.name, entry_point.execution_model);
            reflection.thread_group_sizes[0] = entry.workgroup_size.x;
            reflection.thread_group_sizes[1] = entry.workgroup_size.y;
            reflection.thread_group_sizes[2] = entry.workgroup_size.z;
        }

        if (entry_point.execution_model == spv::ExecutionModel::ExecutionModelVertex)
        {
            uint32_t offset = 0;
            reflection.vertex_inputs.resize(resources.stage_inputs.size());
            for (size_t j = 0; j < resources.stage_inputs.size(); ++j)
            {
                const spirv_cross::Resource& input = resources.stage_inputs[j];
                reflection.vertex_inputs[j].name = input.name.c_str();
                // todo: extract more information
                uint32_t cur_offset = offset;
                const spirv_cross::SPIRType& type = compiler.get_type(input.type_id);
                switch (type.basetype)
                {
                case spirv_cross::SPIRType::BaseType::Float:
                    reflection.vertex_inputs[j].format = static_cast<GPUFormat>(to_underlying(GPUFormat::e_r32_sfloat) + (type.vecsize - 1) * 3);
                    offset += type.vecsize * sizeof(float);
                    break;
                default:
                    break;
                }
                uint32_t location = compiler.get_decoration(input.id, spv::DecorationLocation);
                uint32_t binding = compiler.get_decoration(input.id, spv::DecorationBinding);
            }
        }

        // add more
        reflection.shader_resources.resize(resources.uniform_buffers.size() +
            resources.sampled_images.size() + resources.push_constant_buffers.size() +
            resources.separate_images.size() + resources.separate_samplers.size() +
            resources.storage_buffers.size() + resources.storage_images.size());
        size_t index = 0;
        for (spirv_cross::Resource const& uniform : resources.uniform_buffers)
        {
            GPUShaderResource& ref_resource = reflection.shader_resources[index++];
            ref_resource.name = uniform.name.c_str();
            ref_resource.name_hash = hash_str(uniform.name.c_str(), uniform.name.size(), VK_Hash);
            ref_resource.set = compiler.get_decoration(uniform.id, spv::DecorationDescriptorSet);
            ref_resource.binding = compiler.get_decoration(uniform.id, spv::DecorationBinding);
            ref_resource.stage = transfer_execution_model(entry_point.execution_model);
            ref_resource.resource_type = GPUResourceTypeFlag::e_uniform_buffer;
            ref_resource.size = compiler.get_declared_struct_size(compiler.get_type(uniform.base_type_id));
            ref_resource.texture_type = GPUTextureType::e_undefined;

            const spirv_cross::SPIRType& type = compiler.get_type(uniform.type_id);
            if (!type.array.empty())
            {
                for (const uint32_t& size : type.array)
                    ref_resource.array_count += size;
            }
            else
                ref_resource.array_count = 1;
        }
        for (spirv_cross::Resource const& sampled : resources.sampled_images)
        {
            GPUShaderResource& ref_resource = reflection.shader_resources[index++];
            ref_resource.name = sampled.name.c_str();
            ref_resource.name_hash = hash_str(sampled.name.c_str(), sampled.name.size(), VK_Hash);
            ref_resource.set = compiler.get_decoration(sampled.id, spv::DecorationDescriptorSet);
            ref_resource.binding = compiler.get_decoration(sampled.id, spv::DecorationBinding);
            ref_resource.stage = transfer_execution_model(entry_point.execution_model);
            ref_resource.resource_type = GPUResourceTypeFlag::e_combined_image_sampler;
            ref_resource.size = 0;

            const spirv_cross::SPIRType& type = compiler.get_type(sampled.type_id);
            if (!type.array.empty())
            {
                for (const uint32_t& size : type.array)
                    ref_resource.array_count += size;
            }
            else
                ref_resource.array_count = 1;

            if (type.image.arrayed)
                ref_resource.texture_type = Texture_Array_Type_Map[type.image.dim];
            else
                ref_resource.texture_type = Texture_Type_Map[type.image.dim];

            if (type.image.ms)
            {
                if (ref_resource.texture_type == GPUTextureType::e_2d)
                    ref_resource.texture_type =  GPUTextureType::e_2dms;
                else if (ref_resource.texture_type == GPUTextureType::e_2d_array)
                    ref_resource.texture_type = GPUTextureType::e_2dms_array;
            }
        }
        for (spirv_cross::Resource const& push_constant : resources.push_constant_buffers)
        {
            GPUShaderResource& ref_resource = reflection.shader_resources[index++];
            ref_resource.name = push_constant.name.c_str();
            ref_resource.name_hash = hash_str(push_constant.name.c_str(), push_constant.name.size(), VK_Hash);
            ref_resource.set = 0;
            ref_resource.binding = 0;
            ref_resource.stage = transfer_execution_model(entry_point.execution_model);
            ref_resource.resource_type = GPUResourceTypeFlag::e_push_constant;
            ref_resource.size = compiler.get_declared_struct_size(compiler.get_type(push_constant.base_type_id));
            ref_resource.offset = 0;
        }
        for (spirv_cross::Resource const& image : resources.separate_images)
        {
            GPUShaderResource& ref_resource = reflection.shader_resources[index++];
            ref_resource.name = image.name.c_str();
            ref_resource.name_hash = hash_str(image.name.c_str(), image.name.size(), VK_Hash);
            ref_resource.set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
            ref_resource.binding = compiler.get_decoration(image.id, spv::DecorationBinding);
            ref_resource.stage = transfer_execution_model(entry_point.execution_model);

            const spirv_cross::SPIRType& type = compiler.get_type(image.type_id);
            if (!type.array.empty())
            {
                for (const uint32_t& size : type.array)
                    ref_resource.array_count += size;
            }
            else
                ref_resource.array_count = 1;

            if (type.image.dim == spv::Dim::DimBuffer)
            {
                // buffer
                ref_resource.resource_type = GPUResourceTypeFlag::e_buffer;
                ref_resource.size = 0;
            }
            else
            {
                ref_resource.resource_type = GPUResourceTypeFlag::e_texture;
                ref_resource.size = 0;
                if (type.image.arrayed)
                    ref_resource.texture_type = Texture_Array_Type_Map[type.image.dim];
                else
                    ref_resource.texture_type = Texture_Type_Map[type.image.dim];

                if (type.image.ms)
                {
                    if (ref_resource.texture_type == GPUTextureType::e_2d)
                        ref_resource.texture_type =  GPUTextureType::e_2dms;
                    else if (ref_resource.texture_type == GPUTextureType::e_2d_array)
                        ref_resource.texture_type = GPUTextureType::e_2dms_array;
                }
            }
        }
        for (spirv_cross::Resource const& sampler : resources.separate_samplers)
        {
            GPUShaderResource& ref_resource = reflection.shader_resources[index++];
            ref_resource.name = sampler.name.c_str();
            ref_resource.name_hash = hash_str(sampler.name.c_str(), sampler.name.size(), VK_Hash);
            ref_resource.set = compiler.get_decoration(sampler.id, spv::DecorationDescriptorSet);
            ref_resource.binding = compiler.get_decoration(sampler.id, spv::DecorationBinding);
            ref_resource.stage = transfer_execution_model(entry_point.execution_model);
            ref_resource.resource_type = GPUResourceTypeFlag::e_sampler;
            ref_resource.size = 0;

            const spirv_cross::SPIRType& type = compiler.get_type(sampler.type_id);
            if (!type.array.empty())
            {
                for (const uint32_t& size : type.array)
                    ref_resource.array_count += size;
            }
            else
                ref_resource.array_count = 1;
        }
        for (spirv_cross::Resource const& image : resources.storage_images)
        {
            GPUShaderResource& ref_resource = reflection.shader_resources[index++];
            ref_resource.name = image.name.c_str();
            ref_resource.name_hash = hash_str(image.name.c_str(), image.name.size(), VK_Hash);
            ref_resource.set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
            ref_resource.binding = compiler.get_decoration(image.id, spv::DecorationBinding);
            ref_resource.stage = transfer_execution_model(entry_point.execution_model);

            const spirv_cross::SPIRType& type = compiler.get_type(image.type_id);
            if (!type.array.empty())
            {
                for (const uint32_t& size : type.array)
                    ref_resource.array_count += size;
            }
            else
                ref_resource.array_count = 1;

            if (type.image.dim == spv::Dim::DimBuffer)
            {
                // buffer
                ref_resource.resource_type = GPUResourceTypeFlag::e_rw_buffer;
                ref_resource.size = 0;
            }
            else
            {
                ref_resource.resource_type = GPUResourceTypeFlag::e_rw_texture;
                ref_resource.size = 0;
                if (type.image.arrayed)
                    ref_resource.texture_type = Texture_Array_Type_Map[type.image.dim];
                else
                    ref_resource.texture_type = Texture_Type_Map[type.image.dim];

                if (type.image.ms)
                {
                    if (ref_resource.texture_type == GPUTextureType::e_2d)
                        ref_resource.texture_type =  GPUTextureType::e_2dms;
                    else if (ref_resource.texture_type == GPUTextureType::e_2d_array)
                        ref_resource.texture_type = GPUTextureType::e_2dms_array;
                }
            }
        }
    }

    m_ref_device = device;
}

VKShaderLibrary::~VKShaderLibrary()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    vk_device->m_device_table.vkDestroyShaderModule(vk_device->m_device, m_shader_module, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END