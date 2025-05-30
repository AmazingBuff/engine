//
// Created by AmazingBuff on 2025/5/9.
//

#include "vksurface.h"
#include "vkinstance.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKSurface::VKSurface(GPUInstance const* instance, void* handle, void* hinstance) : m_surface(nullptr)
{
    VKInstance const* vk_instance = static_cast<VKInstance const*>(instance);

#ifdef _WIN64
    VkWin32SurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = static_cast<HINSTANCE>(hinstance),
        .hwnd = static_cast<HWND>(handle),
    };
    VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(vk_instance->m_instance, &create_info, VK_Allocation_Callbacks_Ptr, &m_surface));
#endif
    m_ref_instance = vk_instance->m_instance;
}

VKSurface::~VKSurface()
{
    vkDestroySurfaceKHR(m_ref_instance, m_surface, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END