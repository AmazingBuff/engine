//
// Created by AmazingBuff on 2025/4/25.
//

#include <rendering/rhi/wrapper.h>
#include <SDL3/SDL.h>

#include <fstream>

using namespace Amazing;

Vector<char> read_file(const String& filename)
{
    std::ifstream file(filename.c_str(), std::ios::ate | std::ios::binary);

    if (!file.is_open())
        LOG_ERROR("Read File", "failed to open file!");

    size_t file_size = file.tellg();
    Vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), static_cast<int64_t>(file_size));
    file.close();
    return buffer;
}

void draw(SDL_Window* window)
{
    HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

    GPUInstanceCreateInfo instance_create_info{
        .backend = GPUBackend::e_d3d12,
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
    };

    GPUInstance* instance = GPU_create_instance(instance_create_info);

    Vector<GPUAdapter*> adapters;
    instance->enum_adapters(adapters);

    GPUAdapter* const adapter = adapters[0];


    GPUQueueGroup queue_group{
        .queue_type = GPUQueueType::e_graphics,
        .queue_count = 1
    };
    GPUDeviceCreateInfo device_create_info{
        .disable_pipeline_cache = false,
    };

    device_create_info.queue_groups.emplace_back(queue_group);
    GPUDevice* device = GPU_create_device(adapter, device_create_info);

    GPUQueue const* queue = device->fetch_queue(GPUQueueType::e_graphics, 0);

    GPUFence* present_fence = GPU_create_fence(device);

    GPUSemaphore* present_semaphore = GPU_create_semaphore(device);

    GPUSurface* surface = GPU_create_surface(hwnd);

    GPUSwapChainCreateInfo swap_chain_create_info{
        .width = 960,
        .height = 540,
        .frame_count = 2,
        .format = GPUFormat::e_r8g8b8a8_unorm,
        .enable_vsync = true,
        .surface = surface,
        .present_queues = { queue }
    };

    GPUSwapChain* swap_chain = GPU_create_swap_chain(instance, device, swap_chain_create_info);


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

    GPURootSignature* root_sig = GPU_create_root_signature(device, rs_desc);

    GPUFormat backend_format = GPUFormat::e_r8g8b8a8_unorm;

    GPUGraphicsPipelineCreateInfo pipeline_desc{
        .root_signature = root_sig,
        .vertex_shader = &vertex_shader_entry,
        .fragment_shader = &fragment_shader_entry,
        .color_format = &backend_format,
        .render_target_count = 1,
        .primitive_topology = GPUPrimitiveTopology::e_triangle_list,
    };

    GPUGraphicsPipeline* pipeline = GPU_create_graphics_pipeline(device, pipeline_desc);

    GPU_destroy_shader_library(vertex_shader);
    GPU_destroy_shader_library(fragment_shader);

    GPUCommandPool* command_pool = GPU_create_command_pool(device, queue);

    GPUCommandBufferCreateInfo command_buffer_create_info{};

    GPUCommandBuffer* command_buffer = GPU_create_command_buffer(device, command_pool, command_buffer_create_info);

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
        present_fence->wait();
        uint32_t index = swap_chain->acquire_next_frame(present_semaphore, present_fence);
        GPUTexture const* texture = swap_chain->fetch_back_texture(index);
        GPUTextureView const* texture_view = swap_chain->fetch_back_texture_view(index);

        command_pool->reset();
        command_buffer->begin_command();

        // barrier
        GPUTextureBarrier draw_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_undefined,
            .dst_state = GPUResourceStateFlag::e_render_target,
        };
        GPUResourceBarrierInfo barrier_info{
            .texture_barriers = {draw_barrier}
        };
        command_buffer->resource_barrier(barrier_info);

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

        GPUGraphicsPassEncoder* encoder = command_buffer->begin_graphics_pass(graphics_pass_create_info);

        encoder->bind_pipeline(pipeline);
        encoder->draw(3, 0);

        command_buffer->end_graphics_pass(encoder);

        // barrier
        GPUTextureBarrier present_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_render_target,
            .dst_state = GPUResourceStateFlag::e_present,
        };
        GPUResourceBarrierInfo barrier{
            .texture_barriers = {present_barrier}
        };
        command_buffer->resource_barrier(barrier);

        command_buffer->end_command();

        GPUQueueSubmitInfo queue_submit_info{
            .command_buffers = {command_buffer},
            .wait_semaphores = {present_semaphore}
        };
        queue->submit(queue_submit_info);

        // present
        queue->wait_idle();

        GPUQueuePresentInfo queue_present_info{
            .swap_chain = swap_chain,
            .index = static_cast<uint8_t>(index),
        };
        queue->present(queue_present_info);
    }

    queue->wait_idle();
    present_fence->wait();

    GPU_destroy_fence(present_fence);
    GPU_destroy_semaphore(present_semaphore);

    GPU_destroy_root_signature(root_sig);
    GPU_destroy_command_buffer(command_buffer);
    GPU_destroy_command_pool(command_pool);
    GPU_destroy_graphics_pipeline(pipeline);
    GPU_destroy_swap_chain(swap_chain);

    GPU_destroy_surface(surface);
    GPU_destroy_device(device);
    GPU_destroy_instance(instance);
}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return -1;

    SDL_Window* window = SDL_CreateWindow("Amazing Rendering", 960, 540, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    draw(window);


    int64_t* p = Allocator<int64_t>::allocate(1);


    Allocator<int64_t>::deallocate(p);
    return 0;
}