//
// Created by AmazingBuff on 2025/7/7.
//

#include "common.h"
#include "io/read.h"

using namespace Amazing;

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return -1;

    SDL_Window* window = SDL_CreateWindow("Amazing Rendering", Width, Height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

    load_render_doc_api();

    RenderSystemCreateInfo system_create_info{
        .backend = Backend,
        .window_handle = hwnd,
        .window_width = Width,
        .window_height = Height,
        .format = Backend_Format
    };
    RenderSystem* system = RENDER_create_render_system(system_create_info);

    Triangle3D triangle({ -0.5, -0.5, 0 }, { 0.5, -0.5, 0 }, { 0, 1, 0 });
    Mesh mesh = triangle.as_mesh();

    Node node{
        .transform = Affine3f::Identity(),
        .mesh_indices = {0}
    };
    Scene geometry_scene{
        .root = &node,
        .meshes = {&mesh}
    };

    RenderEntity scene_entity = system->import_scene(geometry_scene);

    Vector<char> shader_code = read_file(RES_DIR"shader/graph_triangle/graph_triangle.hlsl");
    Vector<char> vs_shader = compile_shader(shader_code, L"vs", RenderShaderStage::e_vertex);
    Vector<char> ps_shader = compile_shader(shader_code, L"ps", RenderShaderStage::e_fragment);

    RenderShaderDescriptor vs{
        .code = reinterpret_cast<uint32_t*>(vs_shader.data()),
        .code_size = static_cast<uint32_t>(vs_shader.size()),
        .entry = "vs",
        .stage = RenderShaderStage::e_vertex,
    };
    RenderShaderDescriptor ps{
        .code = reinterpret_cast<uint32_t*>(ps_shader.data()),
        .code_size = static_cast<uint32_t>(ps_shader.size()),
        .entry = "ps",
        .stage = RenderShaderStage::e_fragment,
    };

    RenderShaderDescriptor shaders[] = { vs, ps };


    RenderRasterizerStateDescriptor rasterizer_descriptor{
        .rasterizer_state{
            .cull_mode = RenderCullMode::e_none,
            .fill_mode = RenderFillMode::e_solid,
            .front_face = RenderFrontFace::e_counter_clockwise,
        },
        .color_format = &Backend_Format,
        .depth_stencil_format = Backend_Depth_Format,
        .primitive_topology = RenderPrimitiveTopology::e_triangle_list,
        .render_target_count = 1
    };

    RenderGraphPipelineCreateInfo graph_pipeline_create_info{
        .shaders = shaders,
        .shader_count = 2,
        .rasterizer_descriptor = &rasterizer_descriptor
    };
    RenderEntity pipeline_entity = system->create_pipeline(graph_pipeline_create_info);

    RenderGraphImageCreateInfo image_create_info{
        .width = Width,
        .height = Height,
        .depth = 1,
        .array_layers = 1,
        .mip_levels = 1,
        .format = Backend_Format,
        .usage = RenderGraphImageUsage::e_copy | RenderGraphImageUsage::e_rtv_dsv,
        .layout = RenderGraphImageLayout::e_rtv
    };
    RenderEntity output_texture = system->create_image(image_create_info);

    RenderGraphBufferCreateInfo buffer_create_info{
        .size = 24,
        .format = RenderFormat::e_undefined,
        .usage = RenderGraphBufferUsage::e_cbv,
        .type = RenderGraphBufferType::e_persistent_map
    };
    RenderEntity buffer = system->create_buffer(buffer_create_info);
    float light[] = { 0, 1, 1, 0.5, 0.4, 0.5 };

    RenderGraphCreateInfo graph_create_info{
        .render_system = system,
    };
    RenderGraph* graph = RENDER_create_render_graph(graph_create_info);

    graph->add_pass("triangle", [&](RenderBuilder* builder)
        {
            builder->bind_pipeline(pipeline_entity);
            builder->bind_scene_geometry(scene_entity);
            builder->read("b_light", buffer);
            builder->write("output", output_texture);
        },
        [&](RenderView* view)
        {
            view->set_viewport(0, 0, Width, Height, 0, 1);
            view->set_scissor(0, 0, Width, Height);
            view->set_uniform("b_light", light);
        });

    graph->add_present_pass("ps", output_texture);

    graph->compile();

    RenderSceneCreateInfo scene_create_info{
        .render_system = system,
    };
    RenderScene* scene = RENDER_create_render_scene(scene_create_info);
    scene->add_entity(pipeline_entity);
    scene->add_entity(output_texture);
    scene->add_entity(scene_entity);
    scene->add_entity(buffer);

    scene->attach_graph(graph);

    bool quit = false;
    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                quit = true;
            default:
                break;
            }
        }

        // if (Renderdoc_Api)
        //     Renderdoc_Api->StartFrameCapture(nullptr, nullptr);

        scene->render();


        // if (Renderdoc_Api)
        //     Renderdoc_Api->EndFrameCapture(nullptr, nullptr);
    }

}
