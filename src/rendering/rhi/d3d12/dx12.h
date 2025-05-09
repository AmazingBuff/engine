//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef DX12_H
#define DX12_H

#include <d3d12.h>
#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

static constexpr size_t DX12_Hash = hash_str("DX12", Rendering_Hash);
static constexpr const char* DX12_Memory_Allocation_Name = "D3D12MA";

static constexpr uint32_t DX12_Cpu_Descriptor_Heap_Count[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = { 1024 * 256, 2048, 1024 * 64, 1024 * 64 };
static constexpr uint32_t DX12_Gpu_CbvSrvUav_Descriptor_Heap_Count = 1 << 16;
static constexpr uint32_t DX12_Gpu_Sampler_Descriptor_Heap_Count = 2048;

static constexpr uint32_t DX12_Pipeline_State_Object_Name_Max_Length = 256;

static constexpr D3D_FEATURE_LEVEL D3D12_Feature_Levels[] =
{
    D3D_FEATURE_LEVEL_12_2,
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
};




AMAZING_NAMESPACE_END

#endif //DX12_H
