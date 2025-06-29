//
// Created by AmazingBuff on 2025/4/24.
//

#include "root_signature.h"
#include "shader_library.h"

AMAZING_NAMESPACE_BEGIN

static bool is_static_sampler(GPUShaderResource const& resource, GPURootSignatureCreateInfo const& info)
{
    for (size_t i = 0; i < info.static_samplers.size(); i++)
    {
        if (resource.name == info.static_samplers[i].name)
            return resource.resource_type == GPUResourceType::e_sampler;
    }
    return false;
}

static bool is_push_constant(GPUShaderResource const& resource, GPURootSignatureCreateInfo const& info)
{
    if (resource.resource_type == GPUResourceType::e_push_constant)
        return true;

    for (size_t i = 0; i < info.push_constant_names.size(); i++)
    {
        if (resource.name == info.push_constant_names[i])
            return true;
    }
    return false;
}

static void sort_set_binding(GPUShaderResource const& resource, List<GPUShaderResource>& list)
{
    size_t insert_index = 0;
    for (auto& output_resource : list)
    {
        if ((resource.set > output_resource.set) || (resource.set == output_resource.set && resource.binding > output_resource.binding))
            insert_index++;
        else
            break;
    }
    list.insert(insert_index, resource);
}


static void collect_resource(Vector<GPUShaderResource> const& input_resources, GPURootSignatureCreateInfo const& info,
    List<GPUShaderResource>& output_resources, List<GPUShaderResource>& output_push_constants,
    List<GPUShaderResource>& output_static_samplers)
{
    for (size_t j = 0; j < input_resources.size(); j++)
    {
        GPUShaderResource const& resource = input_resources[j];
        if (is_push_constant(resource, info))
        {
            bool identity = false;
            uint32_t push_constant_size = 0;
            for (auto& push_constant : output_push_constants)
            {
                if (resource.name_hash == push_constant.name_hash &&
                    resource.set == push_constant.set && resource.binding == push_constant.binding &&
                    resource.size == push_constant.size && resource.offset == push_constant.offset)
                {
                    push_constant.stage |= resource.stage;
                    identity = true;
                }
                push_constant_size += push_constant.size;
            }
            if (!identity)
            {
                push_constant_size += resource.size;
                if (push_constant_size > GPU_Max_Push_Constant_Size)
                    RENDERING_LOG_ERROR("push constant size {} exceeds max size 128 bytes!", push_constant_size);
                else
                    sort_set_binding(resource, output_push_constants);
            }
        }
        else if (is_static_sampler(resource, info))
        {
            bool identity = false;
            for (auto& static_sampler : output_static_samplers)
            {
                if (resource.name_hash == static_sampler.name_hash &&
                    resource.set == static_sampler.set && resource.binding == static_sampler.binding)
                {
                    static_sampler.stage |= resource.stage;
                    identity = true;
                }
            }
            if (!identity)
                sort_set_binding(resource, output_static_samplers);
        }
        else
        {
            // resource is ordered /*set -- binding*/
            bool identity = false;
            for (auto& output_resource : output_resources)
            {
                if (resource.set == output_resource.set && resource.binding == output_resource.binding &&
                    resource.resource_type == output_resource.resource_type)
                {
                    output_resource.stage |= resource.stage;
                    identity = true;
                }
            }
            if (!identity)
                sort_set_binding(resource, output_resources);
        }
    }
}

// only for texture and buffer
static GPUResourceState transfer_shader_resource_state(GPUResourceType type, GPUPipelineType pipeline)
{
    switch (type)
    {
    case GPUResourceType::e_buffer:
    case GPUResourceType::e_texture:
    {
        switch (pipeline)
        {
        case GPUPipelineType::e_graphics:
            return GPUResourceState::e_pixel_shader_resource;
        case GPUPipelineType::e_compute:
            return GPUResourceState::e_non_pixel_shader_resource;
        default:
            break;
        }
    }
    case GPUResourceType::e_rw_buffer:
    case GPUResourceType::e_rw_texture:
        return GPUResourceState::e_unordered_access;
    default:
        break;
    }
    return GPUResourceState::e_undefined;
}

GPURootSignature::GPURootSignature() : m_ref_device(nullptr), m_pool(nullptr),
    m_push_constants(nullptr), m_push_constant_count(0), m_pipeline_type(GPUPipelineType::e_graphics) {}

GPURootSignature::~GPURootSignature()
{
    if (m_push_constants)
        Allocator<GPUShaderResource>::deallocate(m_push_constants);
    m_push_constants = nullptr;
    m_push_constant_count = 0;
}

void GPURootSignature::initialize(GPURootSignatureCreateInfo const& info)
{
    GPUShaderReflection const* entry_reflection[32]{};
    // resources
    List<GPUShaderResource> resources;
    List<GPUShaderResource> push_constant_resources;
    List<GPUShaderResource> static_sampler_resources;

    for (size_t i = 0; i < info.shaders.size(); i++)
    {
        GPUShaderEntry const& shader = info.shaders[i];
        for (size_t j = 0; j < shader.library->m_shader_reflections.size(); j++)
        {
            GPUShaderReflection const& reflection = shader.library->m_shader_reflections[j];
            if (!reflection.entry_name.empty() && shader.entry == reflection.entry_name)
            {
                entry_reflection[i] = &reflection;
                break;
            }
        }

        // collect resource
        if (entry_reflection[i] == nullptr)
            entry_reflection[i] = &shader.library->m_shader_reflections[0];

        collect_resource(entry_reflection[i]->shader_resources, info, resources, push_constant_resources, static_sampler_resources);

        if (FLAG_IDENTITY(entry_reflection[i]->stage, GPUShaderStage::e_compute))
            m_pipeline_type = GPUPipelineType::e_compute;
        else if (FLAG_IDENTITY(entry_reflection[i]->stage, GPUShaderStage::e_ray_tracing))
            m_pipeline_type = GPUPipelineType::e_ray_tracing;
        else
            m_pipeline_type = GPUPipelineType::e_graphics;
    }

    // slice according to set count
    Map<uint32_t, Vector<GPUShaderResource>> set_resource_map;
    for (auto& resource : resources)
        set_resource_map[resource.set].push_back(resource);

    m_tables.resize(set_resource_map.size());
    size_t index = 0;
    for (auto& [set, shader_resources] : set_resource_map)
    {
        GPUParameterTable& parameter_table = m_tables[index];
        parameter_table.set_index = set;
        parameter_table.resources = shader_resources;

        index++;
    }

    // push constants
    m_push_constant_count = push_constant_resources.size();
    m_push_constants = Allocator<GPUShaderResource>::allocate(m_push_constant_count);
    index = 0;
    for (auto& push_constant_resource : push_constant_resources)
    {
        m_push_constants[index] = push_constant_resource;
        index++;
    }

    // static samplers
    m_static_samplers.resize(static_sampler_resources.size());
    index = 0;
    for (auto& static_sampler_resource : static_sampler_resources)
    {
        m_static_samplers[index] = static_sampler_resource;
        index++;
    }
}

GPUResourceState GPURootSignature::fetch_shader_resource_state(const String& name) const
{
    GPUResourceState state = GPUResourceState::e_undefined;
    any_of(m_tables, [&state, name, this](GPUParameterTable const& table)
    {
        return any_of(table.resources, [&state, name, this](GPUShaderResource const& resource)
        {
            if (resource.name == name)
            {
                state = transfer_shader_resource_state(resource.resource_type, m_pipeline_type);
                return true;
            }
            return false;
        });
    });

    return state;
}

AMAZING_NAMESPACE_END