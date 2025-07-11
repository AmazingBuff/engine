//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef DRAW_RENDER_VIEW_H
#define DRAW_RENDER_VIEW_H

#include "rendering/draw/render_view.h"
#include "rendering/render_entity.h"
#include "rendering/acceleration/render_command.h"

AMAZING_NAMESPACE_BEGIN

class DrawRenderScene;

class DrawRenderGraphicsView final : public RenderView
{
public:
    DrawRenderGraphicsView(DrawRenderScene const* scene, RenderGraphicsCommand& command);
    ~DrawRenderGraphicsView() override = default;

    void set_uniform(RenderEntity const& entity, uint32_t offset, uint32_t size, void const* data) override;
    void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) override;
    void set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void set_push_constant(String const& name, void const* data) override;
private:
    RenderGraphicsCommand& m_graphics_command;

    friend class DrawRenderScene;
};

class DrawRenderComputeView final : public RenderView
{
public:
    DrawRenderComputeView(DrawRenderScene const* scene, RenderComputeCommand& command);
    ~DrawRenderComputeView() override = default;

    void set_uniform(RenderEntity const& entity, uint32_t offset, uint32_t size, void const* data) override;
    void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) override;
    void set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void set_push_constant(String const& name, void const* data) override;
private:
    RenderComputeCommand& m_compute_command;
};

AMAZING_NAMESPACE_END

#endif //DRAW_RENDER_VIEW_H
