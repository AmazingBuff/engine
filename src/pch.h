//
// Created by AmazingBuff on 2025/4/11.
//

#ifndef PCH_H
#define PCH_H

#include <fstream>
#include <algorithm>

#ifdef _WIN64
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0

#endif //PCH_H
