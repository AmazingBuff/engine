//
// Created by AmazingBuff on 2025/4/23.
//

#ifndef ROOT_SIGNATURE_POOL_H
#define ROOT_SIGNATURE_POOL_H

#include "rendering/rhi/create_info.h"
#include <unordered_map>

AMAZING_NAMESPACE_BEGIN

class GPURootSignaturePool
{
public:
    GPURootSignaturePool() : m_ref_device(nullptr) {}
    virtual ~GPURootSignaturePool();

    virtual AResult initialize(GPUDevice const* device, GPURootSignaturePoolCreateInfo const& info);

    GPURootSignature* find(GPURootSignature const* tables, GPURootSignatureCreateInfo const& info);
    bool remove(GPURootSignature* root_signature);
    GPURootSignature* insert(GPURootSignature* root_signature, GPURootSignatureCreateInfo const& info);
protected:
    GPUDevice const* m_ref_device;

    struct GPURootSignatureCharacteristic
    {
        uint32_t table_count;
        uint64_t table_hash;
        uint32_t push_constant_count;
        uint64_t push_constant_hash;
        uint32_t static_sampler_count;
        uint64_t static_sampler_hash;
        GPUPipelineType pipeline_type;

        NODISCARD bool operator==(const GPURootSignatureCharacteristic& other) const
        {
            return table_count == other.table_count && table_hash == other.table_hash && push_constant_count == other.push_constant_count && push_constant_hash == other.push_constant_hash &&
                static_sampler_count == other.static_sampler_count && static_sampler_hash == other.static_sampler_hash && pipeline_type == other.pipeline_type;
        }

        explicit operator uint64_t() const
        {
            uint64_t seed = Rendering_Hash;
            hash_combine_mul(seed, table_count, table_hash, push_constant_count, push_constant_hash, static_sampler_count, static_sampler_hash, to_underlying(pipeline_type));
            return seed;
        }
    };

    struct GPURootSignatureCharacteristicHasher
    {
        NODISCARD size_t operator()(const GPURootSignatureCharacteristic& val) const noexcept
        {
            return static_cast<size_t>(val);
        }
    };

    struct GPURootSignatureCharacteristicCounter
    {
        GPURootSignatureCharacteristic characteristic;
        uint32_t counter;
    };

    // temporarily use std hash map
    std::unordered_map<GPURootSignatureCharacteristic, GPURootSignature*, GPURootSignatureCharacteristicHasher> m_characteristic_map;
    std::unordered_map<GPURootSignature*, GPURootSignatureCharacteristicCounter> m_reverse_characteristic_map;
private:
    GPURootSignatureCharacteristic transfer_characteristic(GPURootSignature const* tables, GPURootSignatureCreateInfo const& info);

    friend class GPURootSignature;
};


AMAZING_NAMESPACE_END

#endif //ROOT_SIGNATURE_POOL_H
