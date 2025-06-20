//
// Created by AmazingBuff on 2025/5/6.
//

#ifndef VK_EXTS_H
#define VK_EXTS_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

struct GPUVulkanInstanceCreateExtensions
{
    Vector<const char*> layers;
    Vector<const char*> instance_extensions;
    Vector<const char*> device_extensions;
};

AMAZING_NAMESPACE_END

#endif //VK_EXTS_H
