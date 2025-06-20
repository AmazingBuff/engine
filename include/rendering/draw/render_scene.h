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
    RenderScene() : m_ref_render_system(nullptr) {}
    virtual ~RenderScene() = default;

    virtual void add_entity(RenderEntity const& entity);
    virtual void remove_entity(RenderEntity const& entity);
protected:
    RenderSystem const* m_ref_render_system;
};



AMAZING_NAMESPACE_END

#endif //RENDER_SCENE_H
