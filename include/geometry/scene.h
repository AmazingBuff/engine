//
// Created by AmazingBuff on 2025/6/9.
//

#ifndef SCENE_H
#define SCENE_H

#include "3d/primitive/mesh.h"

AMAZING_NAMESPACE_BEGIN

struct Node
{
    String name;
    uint64_t name_hash;
    Affine3f transform;
    Vector<uint32_t> mesh_indices;
    Vector<Node*> children;
};

struct Scene
{
    String name;
    uint64_t name_hash;
    Node* root;
    Vector<Mesh*> meshes;
};

AMAZING_NAMESPACE_END

#endif //SCENE_H
