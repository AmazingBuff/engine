//
// Created by AmazingBuff on 2025/5/7.
//

#ifndef VKDEVICE_H
#define VKDEVICE_H

#include "vk.h"
#include "rendering/rhi/common/device.h"

AMAZING_NAMESPACE_BEGIN

class VKQueue;
class VKDescriptorPool;
class VKPassTable;
class VKCommandPool;
class VKCommandBuffer;

class VKDevice final : public GPUDevice
{
public:
    VKDevice(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info);
    ~VKDevice() override;

    GPUQueue const* fetch_queue(GPUQueueType type, uint32_t index) const override;
private:
    // name must not be empty
    void set_debug_name(size_t handle, VkObjectType type, const String& name) const;
private:
    VkDevice m_device;
    VkPipelineCache m_pipeline_cache;
    VmaAllocator m_allocator;
    VKDescriptorPool* m_descriptor_pool;
    VKPassTable* m_pass_table;
    VolkDeviceTable m_device_table;

    Vector<VKQueue*> m_command_queues[GPU_Queue_Type_Count];

    // internal command pool and command buffer for transfer
    VKCommandPool* m_internal_command_pool;
    VKCommandBuffer* m_internal_command_buffer;

    friend class VKAdapter;
    friend class VKQueue;
    friend class VKCommandPool;
    friend class VKCommandBuffer;
    friend class VKSwapChain;
    friend class VKFence;
    friend class VKSemaphore;
    friend class VKRootSignature;
    friend class VKDescriptorSet;
    friend class VKGraphicsPipeline;
    friend class VKGraphicsPassEncoder;
    friend class VKComputePipeline;
    friend class VKComputePassEncoder;
    friend class VKQueryPool;

    friend class VKBuffer;
    friend class VKTexture;
    friend class VKTextureView;
    friend class VKSampler;
    friend class VKShaderLibrary;
    friend class VKMemoryPool;

    friend class VKDescriptorPool;
    friend class VKPassTable;
};

AMAZING_NAMESPACE_END
#endif //VKDEVICE_H
