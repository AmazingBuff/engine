//
// Created by AmazingBuff on 2025/6/17.
//

#include "render_driver.h"
#include "render_geometry.h"
#include "rendering/rhi/wrapper.h"
#include "rendering/graph/resource/render_graph_resources.h"
#include "rendering/graph/resource/render_util.h"
#include "geometry/geometry.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

AMAZING_NAMESPACE_BEGIN

static RenderMesh import_mesh(const aiMesh* mesh, Vector<RenderVertex>& vertices, Vector<Index3i>& triangles, uint32_t& vertex_offset, uint32_t& triangle_offset)
{
    // position
    if (mesh->HasPositions())
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[vertex_offset + i].position.x() = mesh->mVertices[i].x;
            vertices[vertex_offset + i].position.y() = mesh->mVertices[i].y;
            vertices[vertex_offset + i].position.z() = mesh->mVertices[i].z;
        }
    }

    // normal
    if (mesh->HasNormals())
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[vertex_offset + i].normal.x() = mesh->mNormals[i].x;
            vertices[vertex_offset + i].normal.y() = mesh->mNormals[i].y;
            vertices[vertex_offset + i].normal.z() = mesh->mNormals[i].z;
        }
    }

    // texture coordinate
    if (mesh->HasTextureCoords(0))
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[vertex_offset + i].texcoord.x() = mesh->mTextureCoords[0][i].x;
            vertices[vertex_offset + i].texcoord.y() = mesh->mTextureCoords[0][i].y;
        }
    }

    // tangent
    if (mesh->HasTangentsAndBitangents())
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[vertex_offset + i].tangent.x() = mesh->mTangents[i].x;
            vertices[vertex_offset + i].tangent.y() = mesh->mTangents[i].y;
            vertices[vertex_offset + i].tangent.z() = mesh->mTangents[i].z;
            vertices[vertex_offset + i].bitangent.x() = mesh->mBitangents[i].x;
            vertices[vertex_offset + i].bitangent.y() = mesh->mBitangents[i].y;
            vertices[vertex_offset + i].bitangent.z() = mesh->mBitangents[i].z;
        }
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        Index3i index;
        for (uint32_t j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            index[j] = mesh->mFaces[i].mIndices[j];
        triangles[triangle_offset + i] = index;
    }

    vertex_offset += mesh->mNumVertices;
    triangle_offset += mesh->mNumFaces;

    AABB aabb{
        .min = {mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z},
        .max = {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z}
    };
    return {vertex_offset, triangle_offset * 3, mesh->mNumVertices, mesh->mNumFaces * 3, aabb};
}

static RenderNode* import_node(const aiNode* node, const aiScene* scene, Vector<RenderMesh>& meshes, Vector<RenderVertex>& vertices, Vector<Index3i>& indices, Vector<Affine3f>& transforms, uint32_t mesh_offset, uint32_t& vertex_offset, uint32_t& triangle_offset, uint32_t& transform_offset)
{
    RenderNode *render_node = PLACEMENT_NEW(RenderNode, sizeof(RenderNode));
    memcpy(render_node->transform.model.data(), &node->mTransformation, std::min(sizeof(Affine3f), sizeof(node->mTransformation)));
    render_node->transform.offset = transform_offset * sizeof(Affine3f);
    transforms[transform_offset] = render_node->transform.model;
    transform_offset++;

    render_node->mesh_indices.resize(node->mNumMeshes);
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes[mesh_offset] = import_mesh(mesh, vertices, indices, vertex_offset, triangle_offset);
        render_node->mesh_indices[i] = mesh_offset;
        mesh_offset++;
    }

    render_node->children.resize(node->mNumChildren);
    for (uint32_t i = 0; i < node->mNumChildren; i++)
      render_node->children[i] =
          import_node(node->mChildren[i], scene, meshes, vertices, indices, transforms,
                      mesh_offset, triangle_offset, vertex_offset, transform_offset);

    return render_node;
}

static RenderNode* transfer_render_node(Node* node, uint32_t& transform_offset, Vector<Affine3f>& transforms)
{
    RenderNode* render_node = PLACEMENT_NEW(RenderNode, sizeof(RenderNode));
    render_node->mesh_indices = node->mesh_indices;
    render_node->transform.model = node->transform;
    render_node->transform.offset = transform_offset;

    transform_offset += sizeof(Affine3f);
    transforms.push_back(render_node->transform.model);

    uint32_t child_size = node->children.size();
    render_node->children.resize(child_size);
    for (uint32_t i = 0; i < child_size; i++)
        render_node->children[i] = transfer_render_node(node->children[i], transform_offset, transforms);
    return render_node;
}

static void destroy_render_node(RenderNode* node)
{
    for (RenderNode* child : node->children)
        destroy_render_node(child);
    PLACEMENT_DELETE(RenderNode, node);
}


RenderDriver::RenderDriver(RenderDriverCreateInfo const& info) : m_driver_info{}, m_static_samplers{}
{
    GPUInstanceCreateInfo instance_create_info{
        .backend = info.backend == RenderBackend::e_d3d12 ? GPUBackend::e_d3d12 : GPUBackend::e_vulkan,
#if defined(_DEBUG) || defined(DEBUG)
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
#else
        .enable_debug_layer = false,
        .enable_gpu_based_validation = false,
#endif
    };
    m_instance = GPU_create_instance(instance_create_info);

    Vector<GPUAdapter*> adapters;
    m_instance->enum_adapters(adapters);

    // todo: support multiple adapter
    GPUAdapter* const adapter = adapters[0];

    GPUQueueGroup graphics_queue_group{
        .queue_type = GPUQueueType::e_graphics,
        .queue_count = 1
    };
    GPUQueueGroup compute_queue_group{
        .queue_type = GPUQueueType::e_compute,
        .queue_count = 1
    };
    GPUDeviceCreateInfo device_create_info{
        .disable_pipeline_cache = false,
        .queue_groups = {graphics_queue_group, compute_queue_group},
    };

    m_device = GPU_create_device(adapter, device_create_info);

    constexpr uint32_t filter_count = Reflect::MetaInfo<GPUFilterType>::enum_count();
    constexpr uint32_t address_mode_count = Reflect::MetaInfo<GPUAddressMode>::enum_count();
    for (uint32_t i = 0; i < filter_count; i++)
    {
        for (uint32_t k = 0; k < address_mode_count; k++)
        {
            GPUSamplerCreateInfo sampler_create_info{
                .min_filter = Reflect::MetaInfo<GPUFilterType>::enum_field(i),
                .mag_filter = Reflect::MetaInfo<GPUFilterType>::enum_field(i),
                .mipmap_mode = Reflect::MetaInfo<GPUMipMapMode>::enum_field(i),
                .address_u = Reflect::MetaInfo<GPUAddressMode>::enum_field(k),
                .address_v = Reflect::MetaInfo<GPUAddressMode>::enum_field(k),
                .address_w = Reflect::MetaInfo<GPUAddressMode>::enum_field(k),
                .compare_mode = GPUCompareMode::e_never,
            };
            m_static_samplers[i * address_mode_count + k] = GPU_create_sampler(m_device, sampler_create_info);
        }
    }

    m_driver_info.backend = info.backend;
    m_driver_info.frame_count = info.frame_count;
}

RenderDriver::~RenderDriver()
{
    GPU_destroy_device(m_device);
    GPU_destroy_instance(m_instance);
}

RenderGeometry RenderDriver::import_render_geometry(Scene const& scene) const
{
    uint32_t transform_offset = 0;
    Vector<Affine3f> transforms;
    RenderGeometry geometry;
    geometry.root = transfer_render_node(scene.root, transform_offset, transforms);

    uint32_t mesh_count = scene.meshes.size();
    geometry.meshes.resize(mesh_count);

    uint32_t vertex_count = 0;
    uint32_t triangle_count = 0;
    for (Mesh const* mesh : scene.meshes)
    {
        vertex_count += mesh->vertices.size();
        triangle_count += mesh->triangles.size();
    }
    Vector<RenderVertex> vertices(vertex_count);
    Vector<Index3i> triangles(triangle_count);

    uint32_t vertex_index = 0;
    uint32_t triangle_index = 0;
    for (uint32_t i = 0; i < mesh_count; i++)
    {
        Mesh const* mesh = scene.meshes[i];
        geometry.meshes[i].vertex_offset = vertex_index;
        geometry.meshes[i].index_offset = triangle_index * 3;
        geometry.meshes[i].vertex_count = mesh->vertices.size();
        geometry.meshes[i].index_count = mesh->triangles.size() * 3;
        geometry.meshes[i].aabb = mesh->aabb();

        for (uint32_t j = 0; j < mesh->triangles.size(); j++)
        {
            triangles[triangle_index].x() = mesh->triangles[j].x() + vertex_index;
            triangles[triangle_index].y() = mesh->triangles[j].y() + vertex_index;
            triangles[triangle_index].z() = mesh->triangles[j].z() + vertex_index;
            triangle_index++;
        }

        uint32_t local_vertex_index;

#define COPY_VERTEX_ATTRIBUTE(attribute, val)                   \
        local_vertex_index = vertex_index;                      \
        for (uint32_t j = 0; j < val.size(); j++)               \
        {                                                       \
            vertices[local_vertex_index].attribute = val[j];    \
            local_vertex_index++;                               \
        }

        COPY_VERTEX_ATTRIBUTE(position, mesh->vertices);
        COPY_VERTEX_ATTRIBUTE(normal, mesh->normals);
        COPY_VERTEX_ATTRIBUTE(texcoord, mesh->texcoords);
        COPY_VERTEX_ATTRIBUTE(tangent, mesh->tangents);
        COPY_VERTEX_ATTRIBUTE(bitangent, mesh->bitangents);
#undef COPY_VERTEX_ATTRIBUTE

        vertex_index = local_vertex_index;
    }

    GPUBufferCreateInfo info{
        .size = sizeof(vertices),
        .usage = GPUMemoryUsage::e_cpu_to_gpu,
        .type = GPUResourceType::e_vertex_buffer,
        .flags = GPUBufferFlag::e_dedicated,
    };
    GPUBuffer* vertex_buffer = GPU_create_buffer(m_device, info);
    vertex_buffer->map(0, sizeof(vertices), vertices.data());

    info.size = sizeof(triangles);
    info.type = GPUResourceType::e_index_buffer;
    GPUBuffer* index_buffer = GPU_create_buffer(m_device, info);
    index_buffer->map(0, sizeof(triangles), triangles.data());

    info.size = transform_offset;
    info.type = GPUResourceType::e_uniform_buffer;
    info.flags = GPUBufferFlag::e_persistent_map;
    GPUBuffer* uniform_buffer = GPU_create_buffer(m_device, info);
    uniform_buffer->map(0, transform_offset, transforms.data());

    geometry.vertex_buffer = vertex_buffer;
    geometry.index_buffer = index_buffer;
    geometry.uniform_buffer = uniform_buffer;

    return geometry;
}

RenderGeometry RenderDriver::import_render_geometry(const char* file_name) const
{
    struct GeometryCounter
    {
        uint32_t vertex_count;
        uint32_t triangle_count;
        uint32_t mesh_count;
        uint32_t transform_count;
    };
    // static auto count_geometry_attribute = [](this auto self, const aiNode* node, const aiScene* scene) -> GeometryCounter
    // {
    //     uint32_t vertex_count = 0;
    //     uint32_t triangle_count = 0;
    //     uint32_t mesh_count = 0;
    //     uint32_t transform_count = 1;
    //     for (uint32_t i = 0; i < node->mNumMeshes; i++)
    //     {
    //         aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    //         vertex_count += mesh->mNumVertices;
    //         triangle_count += mesh->mNumFaces;
    //         mesh_count++;
    //     }
    //
    //     for (uint32_t i = 0; i < node->mNumChildren; i++)
    //     {
    //         GeometryCounter count = self(node->mChildren[i], scene);
    //         vertex_count += count.vertex_count;
    //         triangle_count += count.triangle_count;
    //         mesh_count += count.mesh_count;
    //         transform_count += count.transform_count;
    //     }
    //     return {vertex_count, triangle_count, mesh_count, transform_count};
    // };

    uint32_t flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_MakeLeftHanded | aiProcess_GenBoundingBoxes;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file_name, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        LOG_ERROR("Model Loader", "unable to load this model, which is from {}!", file_name);

    GeometryCounter count = {};//count_geometry_attribute(scene->mRootNode, scene);
    Vector<RenderVertex> vertices(count.vertex_count);
    Vector<Index3i> triangles(count.triangle_count);
    Vector<Affine3f> transforms(count.transform_count);

    uint32_t vertex_offset = 0, triangle_offset = 0, mesh_offset = 0, transform_offset = 0;

    RenderGeometry geometry;
    geometry.meshes.reserve(count.mesh_count);
    geometry.root = import_node(scene->mRootNode, scene, geometry.meshes, vertices, triangles, transforms, mesh_offset, vertex_offset, triangle_offset, transform_offset);


    GPUBufferCreateInfo info{
        .size = sizeof(vertices),
        .usage = GPUMemoryUsage::e_cpu_to_gpu,
        .type = GPUResourceType::e_vertex_buffer,
        .flags = GPUBufferFlag::e_dedicated,
    };
    GPUBuffer* vertex_buffer = GPU_create_buffer(m_device, info);
    vertex_buffer->map(0, sizeof(vertices), vertices.data());

    info.size = sizeof(triangles);
    info.type = GPUResourceType::e_index_buffer;
    GPUBuffer* index_buffer = GPU_create_buffer(m_device, info);
    index_buffer->map(0, sizeof(triangles), triangles.data());

    info.size = transform_offset * sizeof(Affine3f);
    info.type = GPUResourceType::e_uniform_buffer;
    info.flags = GPUBufferFlag::e_persistent_map;
    GPUBuffer* uniform_buffer = GPU_create_buffer(m_device, info);
    uniform_buffer->map(0, info.size, transforms.data());

    geometry.vertex_buffer = vertex_buffer;
    geometry.index_buffer = index_buffer;
    geometry.uniform_buffer = uniform_buffer;

    return geometry;
}

void RenderDriver::destroy_render_geometry(RenderGeometry const& geometry) const
{
    GPU_destroy_buffer(const_cast<GPUBuffer*>(geometry.vertex_buffer));
    GPU_destroy_buffer(const_cast<GPUBuffer*>(geometry.index_buffer));
    destroy_render_node(geometry.root);
}

RenderGraphPipeline RenderDriver::create_pipeline(RenderGraphPipelineCreateInfo const& info) const
{
    GPURootSignatureCreateInfo root_signature_info{};
    root_signature_info.shaders.reserve(info.shader_count);
    root_signature_info.push_constant_names.reserve(info.push_constant_count);
    root_signature_info.static_samplers.reserve(info.static_sampler_count);

    GPUShaderEntry const* vertex = nullptr;
    GPUShaderEntry const* tessellation_control = nullptr;
    GPUShaderEntry const* tessellation_evaluation = nullptr;
    GPUShaderEntry const* geometry = nullptr;
    GPUShaderEntry const* fragment = nullptr;
    GPUShaderEntry const* compute = nullptr;

    Vector<GPUShaderLibrary*> libraries(info.shader_count);
    for (uint8_t i = 0; i < info.shader_count; i++)
    {
        GPUShaderStage stage = transfer_shader_stage(info.shaders[i].stage);
        GPUShaderLibraryCreateInfo library_info{
            .code = info.shaders[i].code,
            .code_size = info.shaders[i].code_size,
            .stage = stage,
        };
        libraries[i] = GPU_create_shader_library(m_device, library_info);
        root_signature_info.shaders.emplace_back(libraries[i], info.shaders[i].entry, stage);
#define SHADER_EXTRACT(shader)                                                                                        \
        case GPUShaderStage::e_##shader:                                                                             \
            if (!shader)                                                                                                \
                shader = &root_signature_info.shaders[i];                                                               \
            else                                                                                                        \
                RENDERING_LOG_ERROR("can't use same shader stage twice! shader stage is {}", to_underlying(stage));

        switch (stage)
        {
            SHADER_EXTRACT(vertex);
            SHADER_EXTRACT(tessellation_control);
            SHADER_EXTRACT(tessellation_evaluation);
            SHADER_EXTRACT(geometry);
            SHADER_EXTRACT(fragment);
            SHADER_EXTRACT(compute);
            default:
            RENDERING_LOG_ERROR("unsupported shader stage");
        }

#undef SHADER_EXTRACT
    }

    for (uint8_t i = 0; i < info.push_constant_count; i++)
        root_signature_info.push_constant_names.emplace_back(info.push_constant_names[i]);

    for (uint8_t i = 0; i < info.static_sampler_count; i++)
        root_signature_info.static_samplers.emplace_back(info.static_sampler_names[i],
            m_static_samplers[to_underlying(GPUFilterType::e_linear) * Reflect::MetaInfo<GPUAddressMode>::enum_count() + to_underlying(GPUAddressMode::e_clamp_to_edge)]);

    GPURootSignature* root_signature = GPU_create_root_signature(m_device, root_signature_info);

    RenderGraphPipeline render_pipeline{
        .root_signature = root_signature
    };
    if (info.rasterizer_descriptor)
    {
        // todo: make vertex attribute configurable
        GPUVertexAttribute pos{
            .array_size = 1,
            .format = GPUFormat::e_r32g32b32_sfloat,
            .slot = 0,
            .semantic_name = "POSITION",
            .location = 0,
            .offset = 0,
            .size = 12,
        };

        GPUVertexAttribute tex{
            .array_size = 1,
            .format = GPUFormat::e_r32g32_sfloat,
            .slot = 0,
            .semantic_name = "TEXCOORD",
            .location = 1,
            .offset = 12,
            .size = 8,
        };

        GPUVertexAttribute normal{
            .array_size = 1,
            .format = GPUFormat::e_r32g32b32_sfloat,
            .slot = 0,
            .semantic_name = "NORMAL",
            .location = 2,
            .offset = 20,
            .size = 12,
            .rate = GPUVertexInputRate::e_vertex
        };

        GPUVertexAttribute tangent{
            .array_size = 1,
            .format = GPUFormat::e_r32g32b32_sfloat,
            .slot = 0,
            .semantic_name = "TANGENT",
            .location = 3,
            .offset = 32,
            .size = 12,
        };

        GPURasterizerState rasterizer_state{
            .cull_mode = transfer_cull_mode(info.rasterizer_descriptor->rasterizer_state.cull_mode),
            .fill_mode = transfer_fill_mode(info.rasterizer_descriptor->rasterizer_state.fill_mode),
            .front_face = transfer_front_face(info.rasterizer_descriptor->rasterizer_state.front_face),
        };

        uint8_t render_target_count = info.rasterizer_descriptor->render_target_count;
        GPUFormat* color_formats = STACK_NEW(GPUFormat, render_target_count);
        for (uint8_t i = 0; i < render_target_count; i++)
            color_formats[i] = transfer_format(info.rasterizer_descriptor->color_format[i]);

        GPUGraphicsPipelineCreateInfo graphics_pipeline_info{
            .root_signature = root_signature,
            .vertex_shader = vertex,
            .tessellation_control_shader = tessellation_control,
            .tessellation_evaluation_shader = tessellation_evaluation,
            .geometry_shader = geometry,
            .fragment_shader = fragment,
            .vertex_inputs = { pos, tex, normal, tangent },
            .blend_state = nullptr,
            .depth_stencil_state = nullptr,
            .rasterizer_state = &rasterizer_state,
            .color_format = color_formats,
            .depth_stencil_format = transfer_format(info.rasterizer_descriptor->depth_stencil_format),
            .render_target_count = render_target_count,
            .sample_count = GPUSampleCount::e_1,
            .sample_quality = 0,
            .primitive_topology = transfer_primitive_topology(info.rasterizer_descriptor->primitive_topology),
        };

        GPUGraphicsPipeline* graphics_pipeline = GPU_create_graphics_pipeline(graphics_pipeline_info);

        render_pipeline.pipeline_type = GPUPipelineType::e_graphics;
        render_pipeline.graphics_pipeline = graphics_pipeline;
    }
    else
    {
        GPUComputePipelineCreateInfo compute_pipeline_info{
            .root_signature = root_signature,
            .compute_shader = compute,
        };

        GPUComputePipeline* pipeline = GPU_create_compute_pipeline(compute_pipeline_info);

        render_pipeline.pipeline_type = GPUPipelineType::e_compute;
        render_pipeline.compute_pipeline = pipeline;
    }

    return render_pipeline;
}

void RenderDriver::destroy_pipeline(RenderGraphPipeline const& pipeline) const
{
    switch (pipeline.pipeline_type)
    {
    case GPUPipelineType::e_graphics:
        GPU_destroy_graphics_pipeline(pipeline.graphics_pipeline);
        break;
    case GPUPipelineType::e_compute:
        GPU_destroy_compute_pipeline(pipeline.compute_pipeline);
        break;
    default:
        break;
    }
    GPU_destroy_root_signature(pipeline.root_signature);
}



AMAZING_NAMESPACE_END