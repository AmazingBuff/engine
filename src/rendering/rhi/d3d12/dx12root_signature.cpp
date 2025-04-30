//
// Created by AmazingBuff on 2025/4/24.
//

#include "dx12root_signature.h"
#include "dx12device.h"
#include "resources/dx12sampler.h"
#include "utils/dx_macro.h"
#include "utils/dx_utils.h"
#include "rendering/api.h"
#include "rendering/rhi/common/root_signature_pool.h"

AMAZING_NAMESPACE_BEGIN

DX12RootSignature::DX12RootSignature(GPUDevice const* device, GPURootSignatureCreateInfo const& info) : m_root_signature(nullptr)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);

    initialize(info);
    if (info.pool)
    {
        DX12RootSignature const* pool_root_signature = static_cast<DX12RootSignature const*>(info.pool->find(this, info));
        if (pool_root_signature)
        {
            m_root_signature = pool_root_signature->m_root_signature;
            m_constant_parameters = pool_root_signature->m_constant_parameters;
            m_pool = info.pool;

            return;
        }
    }


    GPUShaderStage shader_stage(GPUShaderStageFlag::e_undefined);
    for (const GPUShaderEntry& shader : info.shaders)
        shader_stage |= shader.stage;

    // flags
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    if (shader_stage & GPUShaderStageFlag::e_vertex)
        flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#define STAGE_FLAG_JUDGE(gpu_stage, flag) if (!(shader_stage & gpu_stage)) flags |= flag;
    STAGE_FLAG_JUDGE(GPUShaderStageFlag::e_vertex, D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS);
    STAGE_FLAG_JUDGE(GPUShaderStageFlag::e_hull, D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
    STAGE_FLAG_JUDGE(GPUShaderStageFlag::e_domain, D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);
    STAGE_FLAG_JUDGE(GPUShaderStageFlag::e_geometry, D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
    STAGE_FLAG_JUDGE(GPUShaderStageFlag::e_fragment, D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
#undef STAGE_FLAG_JUDGE

    // tables
    size_t resource_count = 0;
    for (const auto& [resources, set_index] : m_tables)
        resource_count += resources.size();

    Vector<D3D12_ROOT_PARAMETER1> root_parameters(m_tables.size() + m_push_constants.size());
    Vector<D3D12_DESCRIPTOR_RANGE1> cbv_srv_uav_ranges(resource_count);

    uint32_t valid_root_parameter = 0;
    for (uint32_t i_table = 0, i_range = 0; i_table < m_tables.size(); ++i_table)
    {
        GPUParameterTable& parameter_table = m_tables[i_table];
        D3D12_ROOT_PARAMETER1& root_parameter = root_parameters[valid_root_parameter];
        root_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        const D3D12_DESCRIPTOR_RANGE1& cbv_srv_uav_range_cursor = cbv_srv_uav_ranges[i_range];

        GPUShaderStage stages(GPUShaderStageFlag::e_undefined);
        for (const GPUShaderResource& resource : parameter_table.resources)
        {
            stages |= resource.stage;
            {
                D3D12_DESCRIPTOR_RANGE1& cbv_srv_uav_range = cbv_srv_uav_ranges[i_range];
                cbv_srv_uav_range.RegisterSpace = resource.set;
                cbv_srv_uav_range.BaseShaderRegister = resource.binding;
                cbv_srv_uav_range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
                cbv_srv_uav_range.NumDescriptors = resource.resource_type != GPUResourceTypeFlag::e_uniform_buffer ? resource.size : 0;
                cbv_srv_uav_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                cbv_srv_uav_range.RangeType = transfer_resource_type(resource.resource_type);
                root_parameter.DescriptorTable.NumDescriptorRanges++;
                i_range++;
            }
        }
        if (stages != GPUShaderStageFlag::e_undefined)
        {
            root_parameter.ShaderVisibility = transfer_shader_stage(stages);
            root_parameter.DescriptorTable.pDescriptorRanges = &cbv_srv_uav_range_cursor;
            valid_root_parameter++;
        }
    }

    // push constant
    // todo: add more push constant support
    m_constant_parameters.resize(m_push_constants.size());
    for (uint32_t i = 0; i < m_push_constants.size(); ++i)
    {
        m_constant_parameters[i].root_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        m_constant_parameters[i].root_parameter.Constants.RegisterSpace = m_push_constants[i].set;
        m_constant_parameters[i].root_parameter.Constants.ShaderRegister = m_push_constants[i].binding;
        m_constant_parameters[i].root_parameter.Constants.Num32BitValues = m_push_constants[i].size / sizeof(uint32_t);
        m_constant_parameters[i].root_parameter.ShaderVisibility = transfer_shader_stage(m_push_constants[i].stage);

        root_parameters[valid_root_parameter + i] = m_constant_parameters[i].root_parameter;
        m_constant_parameters[i].index = valid_root_parameter + i;
    }

    // static samplers
    D3D12_STATIC_SAMPLER_DESC* sampler_desc = m_static_samplers.empty() ? nullptr : static_cast<D3D12_STATIC_SAMPLER_DESC*>(alloca(m_static_samplers.size() * sizeof(D3D12_STATIC_SAMPLER_DESC)));
    for (size_t i = 0; i < m_static_samplers.size(); ++i)
    {
        const GPUShaderResource& resource = m_static_samplers[i];
        for (const GPUStaticSampler& sampler : info.static_samplers)
        {
            if (resource.name == sampler.name)
            {
                const D3D12_SAMPLER_DESC& desc = static_cast<DX12Sampler const*>(sampler.sampler)->m_desc;
                sampler_desc[i].Filter = desc.Filter;
                sampler_desc[i].AddressU = desc.AddressU;
                sampler_desc[i].AddressV = desc.AddressV;
                sampler_desc[i].AddressW = desc.AddressW;
                sampler_desc[i].MipLODBias = desc.MipLODBias;
                sampler_desc[i].MaxAnisotropy = desc.MaxAnisotropy;
                sampler_desc[i].ComparisonFunc = desc.ComparisonFunc;
                sampler_desc[i].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                sampler_desc[i].MinLOD = desc.MinLOD;
                sampler_desc[i].MaxLOD = desc.MaxLOD;

                sampler_desc[i].RegisterSpace = resource.set;
                sampler_desc[i].ShaderRegister = resource.binding;
                sampler_desc[i].ShaderVisibility = transfer_shader_stage(resource.stage);
            }
        }
    }

    // serialize
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc{
        .Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
        .Desc_1_1{
            .NumParameters = static_cast<uint32_t>(valid_root_parameter + m_push_constants.size()),
            .pParameters = root_parameters.data(),
            .NumStaticSamplers = static_cast<uint32_t>(m_static_samplers.size()),
            .pStaticSamplers = sampler_desc,
            .Flags = flags,
        }
    };

    ID3DBlob* error_blob = nullptr;
    ID3DBlob* blob = nullptr;
    DX_CHECK_RESULT(D3D12SerializeVersionedRootSignature(&root_signature_desc, &blob, &error_blob));
    DX_CHECK_RESULT(dx12_device->m_device->CreateRootSignature(GPU_Node_Mask, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_root_signature)));

    // insert
    if (info.pool)
    {
        DX12RootSignature* pool_root_signature = static_cast<DX12RootSignature*>(info.pool->insert(this, info));
        if (pool_root_signature)
        {
            m_root_signature = pool_root_signature->m_root_signature;
            m_constant_parameters = pool_root_signature->m_constant_parameters;
            m_pool = info.pool;
        }
    }
}

DX12RootSignature::~DX12RootSignature()
{
    if (m_pool)
        m_pool->remove(this);

    DX_FREE(m_root_signature);
}

AMAZING_NAMESPACE_END