//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#include "rendering/rhi/create_info.h"
#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

GPUFormat transfer_format(RenderFormat format);
GPUShaderStage transfer_shader_stage(RenderShaderStage stage);
GPUCullMode transfer_cull_mode(RenderCullMode cull_mode);
GPUFillMode transfer_fill_mode(RenderFillMode fill_mode);
GPUFrontFace transfer_front_face(RenderFrontFace front_face);
GPUPrimitiveTopology transfer_primitive_topology(RenderPrimitiveTopology topology);

AMAZING_NAMESPACE_END
#endif //RENDER_UTIL_H
