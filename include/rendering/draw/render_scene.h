//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

// a structure which store scene information
class RenderScene
{
public:
    RenderScene() : m_ref_render_system(nullptr), m_ref_render_graph(nullptr), m_ref_render_view(nullptr) {}
    virtual ~RenderScene() = default;

    virtual void add_entity(RenderEntity const& entity) = 0;
    virtual void remove_entity(RenderEntity const& entity) = 0;
    virtual void attach_graph(RenderGraph const* graph) = 0;
    virtual void attach_view(RenderView const* view) = 0;
    virtual void render() = 0;
protected:
    RenderSystem const* m_ref_render_system;
    RenderGraph const* m_ref_render_graph;
    RenderView const* m_ref_render_view;
};



AMAZING_NAMESPACE_END

#endif //RENDER_SCENE_H
