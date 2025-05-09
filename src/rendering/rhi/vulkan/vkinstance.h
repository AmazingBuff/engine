//
// Created by AmazingBuff on 2025/5/6.
//

#ifndef VKINSTANCE_H
#define VKINSTANCE_H

#include "vk.h"
#include "rendering/rhi/common/instance.h"

AMAZING_NAMESPACE_BEGIN

class VKAdapter;

class VKInstance final : public GPUInstance
{
public:
    explicit VKInstance(GPUInstanceCreateInfo const& info);
    ~VKInstance() override;

    void enum_adapters(Vector<GPUAdapter*>& adapters) const override;
private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    Vector<VKAdapter*> m_adapters;

    friend class VKSurface;
    friend class VKDevice;
};

AMAZING_NAMESPACE_END

#endif //VKINSTANCE_H
