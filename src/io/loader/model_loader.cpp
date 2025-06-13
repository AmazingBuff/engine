//
// Created by AmazingBuff on 2025/6/9.
//

#include "io/loader/model_loader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

AMAZING_NAMESPACE_BEGIN

static Mesh* load_mesh(aiMesh* mesh, const aiScene* scene)
{
    Mesh* ret = PLACEMENT_NEW(Mesh, sizeof(Mesh));
    // position
    if (mesh->HasPositions())
    {
        ret->vertices.resize(mesh->mNumVertices);
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            ret->vertices[i].x() = mesh->mVertices[i].x;
            ret->vertices[i].y() = mesh->mVertices[i].y;
            ret->vertices[i].z() = mesh->mVertices[i].z;
        }
    }

    // normal
    if (mesh->HasNormals())
    {
        ret->normals.resize(mesh->mNumVertices);
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            ret->normals[i].x() = mesh->mNormals[i].x;
            ret->normals[i].y() = mesh->mNormals[i].y;
            ret->normals[i].z() = mesh->mNormals[i].z;
        }
    }

    // texture coordinate
    if (mesh->HasTextureCoords(0))
    {
        ret->texcoords.resize(mesh->mNumVertices);
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            ret->texcoords[i].x() = mesh->mTextureCoords[0][i].x;
            ret->texcoords[i].y() = mesh->mTextureCoords[0][i].y;
        }
    }

    // tangent
    if (mesh->HasTangentsAndBitangents())
    {
        ret->tangents.resize(mesh->mNumVertices);
        ret->bitangents.resize(mesh->mNumVertices);
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            ret->tangents[i].x() = mesh->mTangents[i].x;
            ret->tangents[i].y() = mesh->mTangents[i].y;
            ret->tangents[i].z() = mesh->mTangents[i].z;
            ret->bitangents[i].x() = mesh->mBitangents[i].x;
            ret->bitangents[i].y() = mesh->mBitangents[i].y;
            ret->bitangents[i].z() = mesh->mBitangents[i].z;
        }
    }

    ret->triangles.resize(mesh->mNumFaces);
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        Index3i index;
        for (uint32_t j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            index[j] = mesh->mFaces[i].mIndices[j];
        ret->triangles[i] = index;
    }

    return ret;
}

static Node* load_node(aiNode* node, const aiScene* scene, Model& model)
{
    Node* ret = PLACEMENT_NEW(Node, sizeof(Node));
    ret->name = node->mName.data;
    ret->name_hash = hash_str(node->mName.data, node->mName.length);
    memcpy(&ret->transform, &node->mTransformation, std::min(sizeof(ret->transform), sizeof(node->mTransformation)));

    ret->mesh_indices.resize(node->mNumMeshes);
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes.emplace_back(load_mesh(mesh, scene));
        ret->mesh_indices[i] = model.meshes.size() - 1;
    }

    ret->children.resize(node->mNumChildren);
    for (uint32_t i = 0; i < node->mNumChildren; i++)
        ret->children[i] = load_node(node->mChildren[i], scene, model);

    return ret;
}

static void release_node(Node* node)
{
    if (node)
    {
        for (Node* child : node->children)
        {
            release_node(child);
            PLACEMENT_DELETE(Node, child);
        }
    }
}


Model load_model(const char* path)
{
    uint32_t flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_MakeLeftHanded;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        LOG_ERROR("Model Loader", "unable to load this model, which is from {}!", path);

    Model model{};
    model.root = load_node(scene->mRootNode, scene, model);

    return model;
}


void release_model(const Model& model)
{
    release_node(model.root);
    for (Mesh* mesh : model.meshes)
    {
        PLACEMENT_DELETE(Mesh, mesh);
    }
}


AMAZING_NAMESPACE_END