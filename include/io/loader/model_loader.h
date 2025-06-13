//
// Created by AmazingBuff on 2025/6/9.
//

#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <astd/astd.h>
#include "geometry/mesh.h"

AMAZING_NAMESPACE_BEGIN

struct Node
{
    String name;
    uint64_t name_hash;
    Affine3f transform;
    Vector<uint32_t> mesh_indices;
    Vector<Node*> children;
};

struct Model
{
    String name;
    uint64_t name_hash;
    Node* root;
    Vector<Mesh*> meshes;
};

Model load_model(const char* path);
void release_model(const Model& model);


AMAZING_NAMESPACE_END

#endif //MODEL_LOADER_H
