//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef DRAW_RENDER_VIEW_H
#define DRAW_RENDER_VIEW_H

#include "rendering/draw/render_view.h"

AMAZING_NAMESPACE_BEGIN

class DrawRenderView final : public RenderView
{
public:
    explicit DrawRenderView(RenderViewCreateInfo const& info);
    ~DrawRenderView() override = default;


};

AMAZING_NAMESPACE_END

#endif //DRAW_RENDER_VIEW_H
