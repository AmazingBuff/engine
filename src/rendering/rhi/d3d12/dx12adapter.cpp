//
// Created by AmazingBuff on 2025/4/14.
//

#include "dx12adapter.h"
#include "dx12.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12Adapter::DX12Adapter() : m_adapter(nullptr), m_feature_level(D3D_FEATURE_LEVEL_12_0), m_d3d12_detail{} {}

DX12Adapter::~DX12Adapter()
{
    DX_FREE(m_adapter);
}

void DX12Adapter::record_adapter_detail()
{
    DXGI_ADAPTER_DESC3 desc;
    DX_CHECK_RESULT(m_adapter->GetDesc3(&desc));

    m_adapter_detail.vendor.device_id = desc.DeviceId;
    m_adapter_detail.vendor.vendor_id = desc.VendorId;
    sprintf_s(m_adapter_detail.vendor.gpu_name, "%ws", desc.Description);
    // todo: get driver version
    m_adapter_detail.vendor.driver_version = 0;

    // create a device for feature query
    ID3D12Device* device = nullptr;
    DX_CHECK_RESULT(D3D12CreateDevice(m_adapter, m_feature_level, IID_PPV_ARGS(&device)));

    D3D12_FEATURE_DATA_ARCHITECTURE1 architecture;
    DX_CHECK_RESULT(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &architecture, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE1)));
    m_adapter_detail.is_uma = architecture.UMA;
    m_adapter_detail.is_cpu = (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) ? true : false;
    m_adapter_detail.is_virtual = false;

    m_adapter_detail.max_vertex_input_bindings = 32u;
    m_adapter_detail.uniform_buffer_alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    m_adapter_detail.upload_buffer_texture_alignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
    m_adapter_detail.upload_buffer_texture_row_alignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;

    D3D12_FEATURE_DATA_D3D12_OPTIONS options;
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options))))
    {
        m_adapter_detail.support_tiled_volume = (options.TiledResourcesTier >= D3D12_TILED_RESOURCES_TIER_3);
        m_adapter_detail.support_tiled_texture = (options.TiledResourcesTier >= D3D12_TILED_RESOURCES_TIER_1);
        m_adapter_detail.support_tiled_buffer = (options.TiledResourcesTier >= D3D12_TILED_RESOURCES_TIER_1);
    }

    D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1;
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &options1, sizeof(options1))))
    {
        m_adapter_detail.wave_lane_count = options1.WaveLaneCountMin;
    }

    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6;
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, sizeof(options6))))
    {
        if (options6.VariableShadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_1)
        {
            m_adapter_detail.support_shading_rate = true;
            m_adapter_detail.support_shading_rate_mask = false;
            m_adapter_detail.support_shading_rate_sv = true;
        }
        else if (options6.VariableShadingRateTier == D3D12_VARIABLE_SHADING_RATE_TIER_2)
        {
            m_adapter_detail.support_shading_rate = true;
            m_adapter_detail.support_shading_rate_mask = true;
            m_adapter_detail.support_shading_rate_sv = true;
        }
        else
        {
            m_adapter_detail.support_shading_rate = false;
            m_adapter_detail.support_shading_rate_mask = false;
            m_adapter_detail.support_shading_rate_sv = false;
        }
    }

    D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12;
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12))))
    {
        m_d3d12_detail.support_enhanced_barriers = options12.EnhancedBarriersSupported;
    }

    m_adapter_detail.host_visible_vram_budget = 0;
    m_adapter_detail.support_host_visible_vram = false;

    DX_FREE(device);
}

AMAZING_NAMESPACE_END
