//
// Created by AmazingBuff on 2025/6/17.
//

#include "draw_render_system.h"
#include "rendering/acceleration/render_driver.h"
#include "rendering/acceleration/render_asset_manager.h"

AMAZING_NAMESPACE_BEGIN

DrawRenderSystem::DrawRenderSystem(RenderSystemCreateInfo const& info)
{
    RenderDriverCreateInfo driver_info{
        .backend = info.backend,
        .frame_count = RENDER_Frame_Count,
    };
    m_render_driver = PLACEMENT_NEW(RenderDriver, sizeof(RenderDriver), driver_info);

    m_render_asset_manager = PLACEMENT_NEW(RenderAssetManager, sizeof(RenderAssetManager), m_render_driver);
}

DrawRenderSystem::~DrawRenderSystem()
{
    PLACEMENT_DELETE(RenderAssetManager, m_render_asset_manager);
    PLACEMENT_DELETE(RenderDriver, m_render_driver);
}

RenderEntity DrawRenderSystem::import_scene(Scene const& scene)
{
    return m_render_asset_manager->import_scene(scene);
}

RenderEntity DrawRenderSystem::import_scene(const char* file)
{
    return m_render_asset_manager->import_scene(file);
}

RenderEntity DrawRenderSystem::create_texture(RenderGraphTextureCreateInfo const& info)
{
    return {};
}

AMAZING_NAMESPACE_END