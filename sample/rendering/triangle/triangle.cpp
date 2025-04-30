//
// Created by AmazingBuff on 2025/4/25.
//

#include <common.h>

void draw(SDL_Window* window)
{
    HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

    create_api_object(hwnd, GPUBackend::e_d3d12);

    // graphics pipeline
    Vector<char> vs = read_file(RES_DIR"shader/triangle/vertex_shader.dxil");
    Vector<char> fs = read_file(RES_DIR"shader/triangle/fragment_shader.dxil");

    GPUShaderLibraryCreateInfo vs_desc{
        .name = "VertexShaderLibrary",
        .code = reinterpret_cast<uint32_t*>(vs.data()),
        .code_size = static_cast<uint32_t>(vs.size()),
        .stage = GPUShaderStageFlag::e_vertex,
        .reflection = true,
    };
    GPUShaderLibraryCreateInfo fs_desc{
        .name = "FragmentShaderLibrary",
        .code = reinterpret_cast<uint32_t*>(fs.data()),
        .code_size = static_cast<uint32_t>(fs.size()),
        .stage = GPUShaderStageFlag::e_fragment,
        .reflection = true,
    };

    GPUShaderLibrary* vertex_shader = GPU_create_shader_library(vs_desc);

    GPUShaderLibrary* fragment_shader = GPU_create_shader_library(fs_desc);

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

    GPURootSignature* root_sig = GPU_create_root_signature(t_device, rs_desc);

    GPUFormat backend_format = GPUFormat::e_r8g8b8a8_unorm;

    GPUGraphicsPipelineCreateInfo pipeline_desc{
        .root_signature = root_sig,
        .vertex_shader = &vertex_shader_entry,
        .fragment_shader = &fragment_shader_entry,
        .color_format = &backend_format,
        .render_target_count = 1,
        .primitive_topology = GPUPrimitiveTopology::e_triangle_list,
    };

    GPUGraphicsPipeline* pipeline = GPU_create_graphics_pipeline(t_device, pipeline_desc);

    GPU_destroy_shader_library(vertex_shader);
    GPU_destroy_shader_library(fragment_shader);


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
        t_present_fence->wait();
        uint32_t index = t_swap_chain->acquire_next_frame(t_present_semaphore, t_present_fence);
        GPUTexture const* texture = t_swap_chain->fetch_back_texture(index);
        GPUTextureView const* texture_view = t_swap_chain->fetch_back_texture_view(index);

        t_command_pool->reset();
        t_command_buffer->begin_command();

        // barrier
        GPUTextureBarrier draw_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_undefined,
            .dst_state = GPUResourceStateFlag::e_render_target,
        };
        GPUResourceBarrierInfo barrier_info{
            .texture_barriers = {draw_barrier}
        };
        t_command_buffer->resource_barrier(barrier_info);

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
            .depth_stencil_attachment = nullptr,
        };

        GPUGraphicsPassEncoder* encoder = t_command_buffer->begin_graphics_pass(graphics_pass_create_info);

        encoder->set_viewport(0, 0, 960, 540, 0, 1);
        encoder->set_scissor(0, 0, 960, 540);
        encoder->bind_pipeline(pipeline);
        encoder->draw(3, 0);

        t_command_buffer->end_graphics_pass(encoder);

        // barrier
        GPUTextureBarrier present_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_render_target,
            .dst_state = GPUResourceStateFlag::e_present,
        };
        GPUResourceBarrierInfo barrier{
            .texture_barriers = {present_barrier}
        };
        t_command_buffer->resource_barrier(barrier);

        t_command_buffer->end_command();

        GPUQueueSubmitInfo queue_submit_info{
            .command_buffers = {t_command_buffer},
            .signal_fence = t_present_fence
        };
        t_graphics_queue->submit(queue_submit_info);

        // present
        t_graphics_queue->wait_idle();

        GPUQueuePresentInfo queue_present_info{
            .swap_chain = t_swap_chain,
            .index = static_cast<uint8_t>(index),
        };
        t_graphics_queue->present(queue_present_info);
    }

    t_graphics_queue->wait_idle();
    t_present_fence->wait();


    GPU_destroy_root_signature(root_sig);
    GPU_destroy_graphics_pipeline(pipeline);

    destroy_api_object();
}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return -1;

    SDL_Window* window = SDL_CreateWindow("Amazing Rendering", 960, 540, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    draw(window);
    return 0;
}