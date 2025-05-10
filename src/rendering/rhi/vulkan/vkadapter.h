//
// Created by AmazingBuff on 2025/5/6.
//

#ifndef VKADAPTER_H
#define VKADAPTER_H

#include "vk.h"
#include "rendering/rhi/common/adapter.h"

AMAZING_NAMESPACE_BEGIN

class VKAdapter final : public GPUAdapter
{
public:
    ~VKAdapter() override;
    void query_memory_usage(GPUDevice const* device, uint64_t* total, uint64_t* used) override;

public:
    struct VulkanAdapterDetail
    {
        VkPhysicalDeviceProperties device_properties;
    };
    VulkanAdapterDetail m_vulkan_detail;
private:
    VKAdapter();
    void record_adapter_detail();

private:
    VkPhysicalDevice m_physical_device;
    uint32_t m_queue_family_indices[GPU_Queue_Type_Count];

    Vector<const char*> m_device_layers;
    Vector<const char*> m_device_extensions;

    friend class VKInstance;
    friend class VKDevice;
    friend class VKCommandPool;
    friend class VKSwapChain;


    friend class VKBuffer;
    friend class VKTexture;
};

AMAZING_NAMESPACE_END

#endif //VKADAPTER_H
