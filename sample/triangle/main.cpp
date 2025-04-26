//
// Created by AmazingBuff on 2025/4/25.
//

#include <rendering/rhi/d3d12/dx12instance.h>
#include <rendering/rhi/d3d12/dx12adapter.h>
#include <rendering/rhi/d3d12/dx12device.h>

using namespace Amazing;

void create_api_objects()
{
    GPUInstanceCreateInfo instance_create_info{
        .backend = GPUBackend::e_d3d12,
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
    };

    DX12Instance instance;
    instance.initialize(instance_create_info);

    uint32_t num_adapters = 0;
    instance.enum_adapters(nullptr, &num_adapters);
    DX12Adapter const* adapters = nullptr;
    instance.enum_adapters(reinterpret_cast<GPUAdapter const**>(&adapters), &num_adapters);
}

int main()
{
    create_api_objects();

    return 0;
}