//
// Created by AmazingBuff on 2025/4/15.
//

#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "enumeration.h"

AMAZING_NAMESPACE_BEGIN

class GPUSurface;
class GPUInstance;
class GPUAdapter;
class GPUDevice;
class GPUFence;
class GPUSemaphore;
class GPUSwapChain;
class GPUQueue;
class GPUCommandPool;
class GPUCommandBuffer;
class GPUQueryPool;

class GPUBuffer;
class GPUTexture;

struct GPUQueueGroup
{
    GPUQueueType    queue_type;
    uint32_t        queue_count;
};

struct GPUQueryInfo
{
    GPUShaderStage  shader_stage;
    uint32_t        index;
};

struct GPUBufferInfo
{
    size_t size;

};


AMAZING_NAMESPACE_END

#endif //STRUCTURE_H
