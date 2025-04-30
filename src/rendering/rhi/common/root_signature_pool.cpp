//
// Created by AmazingBuff on 2025/4/23.
//
#include "root_signature_pool.h"
#include "root_signature.h"
#include <ranges>

AMAZING_NAMESPACE_BEGIN

struct GPURootSignatureResourceInfo
{
    GPUResourceType resource_type;
    GPUTextureType texture_type;
    GPUShaderStage stage;
    uint32_t set;
    uint32_t binding;
    uint32_t size;
    uint32_t offset;

    explicit operator size_t() const
    {
        size_t hash = hash_combine(Rendering_Hash, to_underlying(resource_type));
        hash = hash_combine(hash, to_underlying(texture_type));
        hash = hash_combine(hash, to_underlying(stage));
        hash = hash_combine(hash, set);
        hash = hash_combine(hash, binding);
        hash = hash_combine(hash, size);
        hash = hash_combine(hash, offset);
        return hash;
    }
};

struct GPUStaticSamplerInfo
{
    uint32_t set;
    uint32_t binding;
    GPUSampler const* id;

    explicit operator size_t() const
    {
        size_t hash = hash_combine(Rendering_Hash, set);
        hash = hash_combine(hash, binding);
        hash = hash_combine(hash, reinterpret_cast<uint64_t>(id));
        return hash;
    }
};

struct GPUPushConstantInfo
{
    GPUShaderStage stage;
    uint32_t set;
    uint32_t binding;
    uint32_t size;
    uint32_t offset;

    explicit operator size_t() const
    {
        size_t hash = hash_combine(Rendering_Hash, to_underlying(stage));
        hash = hash_combine(hash, set);
        hash = hash_combine(hash, binding);
        hash = hash_combine(hash, size);
        hash = hash_combine(hash, offset);
        return hash;
    }
};


GPURootSignaturePool::GPURootSignatureCharacteristic GPURootSignaturePool::transfer_characteristic(GPURootSignature const* tables, GPURootSignatureCreateInfo const& info) const
{
    GPURootSignatureCharacteristic characteristic{
        .table_count = static_cast<uint32_t>(tables->m_tables.size()),
        .table_hash = reinterpret_cast<uint64_t>(this),
    };
    for (size_t i = 0; i < tables->m_tables.size(); i++)
    {
        for (size_t j = 0; j < tables->m_tables[i].resources.size(); j++)
        {
            GPUShaderResource const& resource = tables->m_tables[i].resources[j];
            GPURootSignatureResourceInfo root_signature_resource{
                .resource_type = resource.resource_type,
                .texture_type = resource.texture_type,
                .stage = resource.stage,
                .binding = resource.binding,
                .size = resource.size,
                .offset = resource.offset
            };
            characteristic.table_hash = hash_combine(characteristic.table_hash, static_cast<uint64_t>(root_signature_resource));
        }
    }

    characteristic.push_constant_count = tables->m_push_constants.size();
    characteristic.push_constant_hash = reinterpret_cast<uint64_t>(this);
    for (size_t i = 0; i < tables->m_push_constants.size(); i++)
    {
        GPUShaderResource const& resource = tables->m_push_constants[i];
        GPUPushConstantInfo push_constant{
            .stage = resource.stage,
            .set = resource.set,
            .binding = resource.binding,
            .size = resource.size,
            .offset = resource.offset
        };
        characteristic.push_constant_hash = hash_combine(characteristic.push_constant_hash, static_cast<uint64_t>(push_constant));
    }

    characteristic.static_sampler_count = info.static_samplers.size();
    characteristic.static_sampler_hash = ~0;
    for (size_t i = 0; i < tables->m_static_samplers.size(); i++)
    {
        for (size_t j = 0; j < info.static_samplers.size(); j++)
        {
            if (tables->m_static_samplers[i].name == info.static_samplers[j].name)
            {
                GPUShaderResource const& resource = tables->m_static_samplers[i];
                GPUStaticSamplerInfo sampler_info{
                    .set = resource.set,
                    .binding = resource.binding,
                    .id = info.static_samplers[j].sampler,
                };
                characteristic.static_sampler_hash = hash_combine(characteristic.static_sampler_hash, static_cast<uint64_t>(sampler_info));
            }
        }
    }

    characteristic.pipeline_type = tables->m_pipeline_type;
    return characteristic;
}

GPURootSignature* GPURootSignaturePool::find(GPURootSignature const* tables, GPURootSignatureCreateInfo const& info)
{
    GPURootSignatureCharacteristic characteristic = transfer_characteristic(tables, info);
    auto iter = m_characteristic_map.find(characteristic);
    if (iter != m_characteristic_map.end())
    {
        m_reverse_characteristic_map[iter->second].counter++;
        return iter->second;
    }
    return nullptr;
}

bool GPURootSignaturePool::remove(GPURootSignature* root_signature)
{
    auto iter = m_reverse_characteristic_map.find(root_signature);
    if (iter != m_reverse_characteristic_map.end())
    {
        auto& [characteristic, counter] = iter->second;
        if (counter <= 1)
        {
            m_reverse_characteristic_map.erase(iter);
            m_characteristic_map.erase(characteristic);
            return true;
        }
        counter--;
        return true;
    }
    return false;
}

GPURootSignature* GPURootSignaturePool::insert(GPURootSignature* root_signature, GPURootSignatureCreateInfo const& info)
{
    GPURootSignatureCharacteristic characteristic = transfer_characteristic(root_signature, info);
    auto iter = m_characteristic_map.find(characteristic);
    if (iter != m_characteristic_map.end())
    {
        m_reverse_characteristic_map[iter->second].counter++;
        return iter->second;
    }
    m_characteristic_map[characteristic] = root_signature;
    m_reverse_characteristic_map[root_signature] = {characteristic, 1};
    return root_signature;
}

AMAZING_NAMESPACE_END