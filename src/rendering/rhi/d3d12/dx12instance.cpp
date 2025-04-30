//
// Created by AmazingBuff on 2025/4/14.
//

#include "dx12instance.h"
#include "dx12adapter.h"
#include "dx12.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12Instance::DX12Instance(GPUInstanceCreateInfo const& info) : m_dxgi_factory(nullptr), m_debug(nullptr)
{
    UINT flags = 0;
    if (info.enable_debug_layer)
    {
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug))))
        {
            m_debug->EnableDebugLayer();
            if (info.enable_gpu_based_validation)
            {
                ID3D12Debug1* debug;
                if (SUCCEEDED(m_debug->QueryInterface(IID_PPV_ARGS(&debug))))
                {
                    debug->SetEnableGPUBasedValidation(info.enable_gpu_based_validation);
                    DX_FREE(debug);
                }
            }
        }
        flags = DXGI_CREATE_FACTORY_DEBUG;
    }

    DX_CHECK_RESULT(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_dxgi_factory)));

    struct AdapterInfo
    {
        IDXGIAdapter4* adapter;
        D3D_FEATURE_LEVEL feature_level;
    };

    Vector<AdapterInfo> adapter_infos;
    IDXGIAdapter4* adapter = nullptr;
    for (uint32_t index = 0; m_dxgi_factory->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; index++)
    {
        DXGI_ADAPTER_DESC3 desc;
        DX_CHECK_RESULT(adapter->GetDesc3(&desc));
        if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
        {
            for (const D3D_FEATURE_LEVEL& feature_level : D3D12_Feature_Levels)
            {
                ID3D12Device* device;
                if (SUCCEEDED(D3D12CreateDevice(adapter, feature_level, IID_PPV_ARGS(&device))))
                {
                    DX_FREE(device);

                    adapter_infos.emplace_back(adapter, feature_level);
                    break;
                }
            }
        }
        else
        {
            if (!adapter_infos.empty())
                RENDERING_LOG_WARNING("software rendering will not be added to adapter list!");
            else
                RENDERING_LOG_ERROR("can't support software rendering!");
            DX_FREE(adapter);
        }
    }

    m_adapters.resize(adapter_infos.size());
    for (uint32_t i = 0; i < adapter_infos.size(); i++)
    {
        DX12Adapter* dx12_adapter = PLACEMENT_NEW(DX12Adapter, sizeof(DX12Adapter), nullptr);
        dx12_adapter->m_adapter = adapter_infos[i].adapter;
        dx12_adapter->m_feature_level = adapter_infos[i].feature_level;
        dx12_adapter->record_adapter_detail();
        m_adapters[i] = dx12_adapter;
    }
}

DX12Instance::~DX12Instance()
{
    for (size_t i = 0; i < m_adapters.size(); i++)
    {
        PLACEMENT_DELETE(DX12Adapter, m_adapters[i]);
        m_adapters[i] = nullptr;
    }

    DX_FREE(m_dxgi_factory);
    DX_FREE(m_debug);
}

void DX12Instance::enum_adapters(Vector<GPUAdapter*>& adapters) const
{
    adapters.resize(m_adapters.size());
    memcpy(adapters.data(), m_adapters.data(), sizeof(GPUAdapter*) * m_adapters.size());
}


AMAZING_NAMESPACE_END