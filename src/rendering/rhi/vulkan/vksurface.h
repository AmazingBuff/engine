//
// Created by AmazingBuff on 2025/5/9.
//

#ifndef VKSURFACE_H
#define VKSURFACE_H

#include "vk.h"
#include "vkinstance.h"

AMAZING_NAMESPACE_BEGIN

class VKSurface final
{
public:
    VKSurface(GPUInstance const* instance, void* handle);
    ~VKSurface();
private:
    VkSurfaceKHR m_surface;
    VkInstance m_ref_instance;
};

AMAZING_NAMESPACE_END

#endif //VKSURFACE_H
