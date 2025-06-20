//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef DRAW_RENDER_SYSTEM_H
#define DRAW_RENDER_SYSTEM_H

#include "rendering/draw/render_system.h"

AMAZING_NAMESPACE_BEGIN

class RenderDriver;
class RenderAssetManager;

class DrawRenderSystem final : public RenderSystem
{
public:
    explicit DrawRenderSystem(RenderSystemCreateInfo const& info);
    ~DrawRenderSystem() override;

    RenderEntity import_scene(Scene const& scene) override;
    RenderEntity import_scene(const char *file) override;

    RenderEntity create_texture(RenderGraphTextureCreateInfo const& info) override;
private:
    RenderDriver* m_render_driver;
    RenderAssetManager* m_render_asset_manager;
};

AMAZING_NAMESPACE_END
#endif //DRAW_RENDER_SYSTEM_H
