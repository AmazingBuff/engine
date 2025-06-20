//
// Created by AmazingBuff on 2025/6/9.
//

#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "geometry/mesh.h"

AMAZING_NAMESPACE_BEGIN

Scene LOADER_load_scene(const char* path);
void LOADER_release_scene(const Scene& model);

AMAZING_NAMESPACE_END

#endif //SCENE_LOADER_H
