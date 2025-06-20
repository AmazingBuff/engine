//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_ASSET_MANAGER_H
#define RENDER_ASSET_MANAGER_H

#include "render_geometry.h"
#include "rendering/draw/render_entity.h"
#include "rendering/graph/resource/render_graph_texture.h"

AMAZING_NAMESPACE_BEGIN

class RenderDriver;

class RenderAssetManager
{
public:
    explicit RenderAssetManager(RenderDriver const* driver);

    RenderEntity import_scene(Scene const& scene);
    RenderEntity import_scene(const char* file);

private:
    RenderDriver const* m_ref_render_driver;
    HashMap<RenderEntity, RenderGeometry> m_render_geometries;
};

AMAZING_NAMESPACE_END

#endif //RENDER_ASSET_MANAGER_H
