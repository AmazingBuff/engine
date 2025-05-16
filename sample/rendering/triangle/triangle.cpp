//
// Created by AmazingBuff on 2025/4/25.
//

#include <common.h>

GPURootSignature* root_signature = nullptr;
GPUGraphicsPipeline* pipeline = nullptr;

void create_pipeline()
{
    // graphics pipeline
    Vector<char> vs = read_file(RES_DIR"shader/triangle/vertex_shader.hlsl");
    Vector<char> fs = read_file(RES_DIR"shader/triangle/fragment_shader.hlsl");

    Vector<char> vs_compile = compile_shader(vs, L"main", GPUShaderStageFlag::e_vertex);
    Vector<char> fs_compile = compile_shader(fs, L"main", GPUShaderStageFlag::e_fragment);

    GPUShaderLibraryCreateInfo vs_desc{
        .name = "VertexShaderLibrary",
        .code = reinterpret_cast<uint32_t*>(vs_compile.data()),
        .code_size = static_cast<uint32_t>(vs_compile.size()),
        .stage = GPUShaderStageFlag::e_vertex,
        .reflection = true,
    };
    GPUShaderLibraryCreateInfo fs_desc{
        .name = "FragmentShaderLibrary",
        .code = reinterpret_cast<uint32_t*>(fs_compile.data()),
        .code_size = static_cast<uint32_t>(fs_compile.size()),
        .stage = GPUShaderStageFlag::e_fragment,
        .reflection = true,
    };

    GPUShaderLibrary* vertex_shader = GPU_create_shader_library(t_device, vs_desc);

    GPUShaderLibrary* fragment_shader = GPU_create_shader_library(t_device, fs_desc);

    GPUShaderEntry vertex_shader_entry{
        .library = vertex_shader,
        .entry = "main",
        .stage = GPUShaderStageFlag::e_vertex,
    };

    GPUShaderEntry fragment_shader_entry{
        .library = fragment_shader,
        .entry = "main",
        .stage = GPUShaderStageFlag::e_fragment,
    };

    GPURootSignatureCreateInfo rs_desc{
        .shaders = { vertex_shader_entry, fragment_shader_entry },
    };

    root_signature = GPU_create_root_signature(t_device, rs_desc);

    GPUFormat backend_format = GPUFormat::e_r8g8b8a8_unorm;

    GPUGraphicsPipelineCreateInfo pipeline_desc{
        .root_signature = root_signature,
        .vertex_shader = &vertex_shader_entry,
        .fragment_shader = &fragment_shader_entry,
        .color_format = &backend_format,
        .render_target_count = 1,
        .primitive_topology = GPUPrimitiveTopology::e_triangle_list,
    };

    pipeline = GPU_create_graphics_pipeline(pipeline_desc);

    GPU_destroy_shader_library(vertex_shader);
    GPU_destroy_shader_library(fragment_shader);
}

void destroy_pipeline()
{
    GPU_destroy_root_signature(root_signature);
    GPU_destroy_graphics_pipeline(pipeline);
}

void draw(SDL_Window* window)
{
    HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
    HINSTANCE hinstance = static_cast<HINSTANCE>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr));

    create_api_object(hwnd, hinstance, GPUBackend::e_vulkan);
    create_pipeline();

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
                break;
            }
        }

        // draw
        uint32_t index = t_swap_chain->acquire_next_frame(t_image_semaphore, nullptr);
        GPUTexture const* texture = t_swap_chain->fetch_back_texture(index);
        GPUTextureView const* texture_view = t_swap_chain->fetch_back_texture_view(index);

        t_present_fence[index]->wait();
        t_command_pool[index]->reset();
        t_command_buffer[index]->begin_command();

        // barrier
        GPUTextureBarrier draw_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_undefined,
            .dst_state = GPUResourceStateFlag::e_render_target,
        };
        GPUResourceBarrierInfo barrier_info{
            .texture_barriers = {draw_barrier}
        };
        t_command_buffer[index]->resource_barrier(barrier_info);

        GPUColorAttachment color_attachment{
            .texture_view = texture_view,
            .load = GPULoadAction::e_clear,
            .store = GPUStoreAction::e_store,
            .clear_color = {0, 0, 0, 1}
        };

        GPUGraphicsPassCreateInfo graphics_pass_create_info{
            .name = "triangle",
            .sample_count = GPUSampleCount::e_1,
            .color_attachments = {color_attachment},
            .color_attachment_count = 1,
            .depth_stencil_attachment = nullptr,
        };

        GPUGraphicsPassEncoder* encoder = t_command_buffer[index]->begin_graphics_pass(graphics_pass_create_info);

        encoder->set_viewport(Width / 4, 0, Width / 2, Height, 0, 1);
        encoder->set_scissor(0, 0, Width, Height);
        encoder->bind_pipeline(pipeline);
        encoder->draw(3, 0);

        t_command_buffer[index]->end_graphics_pass(encoder);

        // barrier
        GPUTextureBarrier present_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_render_target,
            .dst_state = GPUResourceStateFlag::e_present,
        };
        GPUResourceBarrierInfo barrier{
            .texture_barriers = {present_barrier}
        };
        t_command_buffer[index]->resource_barrier(barrier);

        t_command_buffer[index]->end_command();

        GPUQueueSubmitInfo queue_submit_info{
            .command_buffers = {t_command_buffer[index]},
            .wait_semaphores = {t_image_semaphore},
            .signal_semaphores = {t_present_semaphore},
            .signal_fence = t_present_fence[index],
        };
        t_graphics_queue->submit(queue_submit_info);

        // present
        t_graphics_queue->wait_idle();

        GPUQueuePresentInfo queue_present_info{
            .swap_chain = t_swap_chain,
            .wait_semaphores = {t_present_semaphore},
            .index = static_cast<uint8_t>(index),
        };
        t_graphics_queue->present(queue_present_info);
    }

    t_graphics_queue->wait_idle();
    for (uint32_t i = 0; i < Frame_In_Flight; i++)
        t_present_fence[i]->wait();


    destroy_pipeline();
    destroy_api_object();
}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return -1;

    SDL_Window* window = SDL_CreateWindow("Amazing Rendering", Width, Height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    draw(window);

    return 0;
}