//
// Created by AmazingBuff on 2025/6/17.
//

#include "render_asset_manager.h"
#include "render_driver.h"
#include "rendering/rhi/wrapper.h"

AMAZING_NAMESPACE_BEGIN
    static RenderEntity generate_render_entity()
{
    static std::atomic<uint32_t> id = 1;
    RenderEntity ret = *reinterpret_cast<RenderEntity*>(&id);
    ++id;
    return ret;
}

RenderAssetManager::RenderAssetManager(RenderDriver const* driver)
{
    m_ref_render_driver = driver;
}

RenderEntity RenderAssetManager::import_scene(Scene const& scene)
{
    RenderGeometry geometry = GPU_import_render_geometry(m_ref_render_driver->m_device, scene);
    RenderEntity entity = generate_render_entity();
    m_render_geometries[entity] = std::move(geometry);
    return entity;
}

RenderEntity RenderAssetManager::import_scene(const char* file)
{
    RenderGeometry geometry = GPU_import_render_geometry(m_ref_render_driver->m_device, file);
    RenderEntity entity = generate_render_entity();
    m_render_geometries[entity] = std::move(geometry);
    return entity;
}

AMAZING_NAMESPACE_END