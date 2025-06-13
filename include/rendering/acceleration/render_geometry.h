//
// Created by AmazingBuff on 2025/6/12.
//

#pragma once

#include "rendering/api.h"
#include "rendering/rhi/create_info.h"
#include "io/loader/model_loader.h"

AMAZING_NAMESPACE_BEGIN

struct GPURenderGeometry
{
    GPUBuffer const* vertex_buffer;
    GPUBuffer const* index_buffer;

    RenderNode* root;
    Vector<RenderMesh> meshes;
};

GPURenderGeometry GPU_import_render_geometry(GPUDevice const* device, Model const& model);
GPURenderGeometry GPU_import_render_geometry(GPUDevice const* device, const char* file);
void GPU_destroy_render_geometry(GPURenderGeometry const& geometry);

AMAZING_NAMESPACE_END