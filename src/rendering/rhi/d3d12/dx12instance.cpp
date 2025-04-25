//
// Created by AmazingBuff on 2025/4/14.
//

#include "dx12instance.h"
#include "dx12adapter.h"
#include "dx12.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12Instance::DX12Instance() : m_dxgi_factory(nullptr), m_debug(nullptr) {}

DX12Instance::~DX12Instance()
{
    DX_FREE(m_dxgi_factory);
    DX_FREE(m_debug);
}


AResult DX12Instance::initialize(GPUInstanceCreateInfo const& info)
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

    IDXGIAdapter4* adapter = nullptr;
    for (uint32_t index = 0; SUCCEEDED(m_dxgi_factory->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter))); index++)
    {
        DXGI_ADAPTER_DESC3 desc;
        DX_CHECK_RESULT(adapter->GetDesc3(&desc));
        if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
        {
            for (const D3D_FEATURE_LEVEL D3D12_Feature_Level : D3D12_Feature_Levels)
            {
                ID3D12Device* device;
                if (SUCCEEDED(D3D12CreateDevice(adapter, D3D12_Feature_Level, IID_PPV_ARGS(&device))))
                {
                    DX_FREE(device);

                    DX12Adapter dx12_adapter;
                    if (SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&dx12_adapter.m_adapter))))
                    {
                        dx12_adapter.m_feature_level = D3D12_Feature_Level;
                        dx12_adapter.record_adapter_detail();

                        m_adapters.push_back(std::move(dx12_adapter));
                        break;
                    }
                }
            }
        }
        else
            RENDERING_LOG_ERROR("can't support software rendering!");

        DX_FREE(adapter);
    }

    return AResult::e_succeed;
}

void DX12Instance::enum_adapters(const GPUAdapter** const adapters, uint32_t* num_adapters) const
{
    *adapters = m_adapters.data();
    *num_adapters = m_adapters.size();
}


AMAZING_NAMESPACE_END