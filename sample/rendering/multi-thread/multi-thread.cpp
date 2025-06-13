//
// Created by AmazingBuff on 2025/6/9.
//


#include <common.h>
#include <condition_variable>

GPUInstance* instance = nullptr;
GPUDevice* device = nullptr;
GPUQueue* graphics_queue = nullptr;
GPUQueue* compute_queue = nullptr;
GPUSwapChain* swap_chain = nullptr;
GPUSurface* surface = nullptr;
GPUCommandPool* command_pool_for_work[Frame_In_Flight] {};
GPUCommandPool* command_pool_for_present[Frame_In_Flight] {};
GPUCommandPool* compute_pool[Frame_In_Flight] {};

GPURootSignature* root_signature = nullptr;
GPURootSignature* compute_root_signature = nullptr;
GPURootSignature* back_root_signature = nullptr;
GPUGraphicsPipeline* pipeline = nullptr;
GPUComputePipeline* compute_pipeline = nullptr;
GPUGraphicsPipeline* back_pipeline = nullptr;

GPUTexture* texture = nullptr;
GPUTextureView* texture_view = nullptr;

GPUTexture* rw_texture = nullptr;
GPUTextureView* rw_texture_view = nullptr;
GPUDescriptorSet* compute_srv_set = nullptr;
GPUDescriptorSet* compute_uav_set = nullptr;
GPUDescriptorSet* second_compute_srv_set = nullptr;
GPUDescriptorSet* second_compute_uav_set = nullptr;

GPUSampler* sampler = nullptr;
GPUDescriptorSet* back_texture_set = nullptr;

GPUSemaphore* thread_semaphore[Frame_In_Flight]{};

uint32_t index = 0;
std::condition_variable condition;
std::mutex mutex;
std::mutex submit_mutex;
bool quit = false;
bool waiting = true;
bool work_running = false;
bool present_running = false;

void create_pipeline()
{
    // graphics pipeline
    GPUTextureCreateInfo texture_create_info{
        .width = Width,
        .height = Height,
        .depth = 1,
        .array_layers = 1,
        .mip_levels = 1,
        .sample_quality = 0,
        .sample_count = GPUSampleCount::e_1,
        .format = Backend_Format,
        .state = GPUResourceStateFlag::e_render_target,
        .type = GPUResourceTypeFlag::e_render_target,
        .flags = GPUTextureFlagsFlag::e_dedicated
    };
    texture_create_info.type |= GPUResourceTypeFlag::e_texture;
    texture_create_info.type |= GPUResourceTypeFlag::e_rw_texture;
    texture = GPU_create_texture(device, texture_create_info);

    GPUTextureViewCreateInfo texture_view_create_info{
        .texture = texture,
        .format = Backend_Format,
        .usage = GPUTextureViewUsageFlag::e_rtv_dsv,
        .aspect = GPUTextureViewAspectFlag::e_color,
        .type = GPUTextureType::e_2d,
        .base_array_layer = 0,
        .array_layers = 1,
        .base_mip_level = 0,
        .mip_levels = 1
    };
    texture_view_create_info.usage |= GPUTextureViewUsageFlag::e_srv;
    texture_view_create_info.usage |= GPUTextureViewUsageFlag::e_uav;
    texture_view = GPU_create_texture_view(texture_view_create_info);

    Vector<char> shader = read_file(RES_DIR"shader/triangle/triangle.hlsl");

    Vector<char> vs_compile = compile_shader(shader, L"vs", GPUShaderStageFlag::e_vertex);
    Vector<char> fs_compile = compile_shader(shader, L"ps", GPUShaderStageFlag::e_fragment);

    GPUShaderLibraryCreateInfo vs_desc{
        .code = reinterpret_cast<uint32_t*>(vs_compile.data()),
        .code_size = static_cast<uint32_t>(vs_compile.size()),
        .stage = GPUShaderStageFlag::e_vertex,
    };
    GPUShaderLibraryCreateInfo fs_desc{
        .code = reinterpret_cast<uint32_t*>(fs_compile.data()),
        .code_size = static_cast<uint32_t>(fs_compile.size()),
        .stage = GPUShaderStageFlag::e_fragment,
    };

    GPUShaderLibrary* vertex_shader = GPU_create_shader_library(device, vs_desc);

    GPUShaderLibrary* fragment_shader = GPU_create_shader_library(device, fs_desc);

    GPUShaderEntry vertex_shader_entry{
        .library = vertex_shader,
        .entry = "vs",
        .stage = GPUShaderStageFlag::e_vertex,
    };

    GPUShaderEntry fragment_shader_entry{
        .library = fragment_shader,
        .entry = "ps",
        .stage = GPUShaderStageFlag::e_fragment,
    };

    GPURootSignatureCreateInfo rs_desc{
        .shaders = { vertex_shader_entry, fragment_shader_entry },
    };

    root_signature = GPU_create_root_signature(device, rs_desc);

    GPUGraphicsPipelineCreateInfo pipeline_desc{
        .root_signature = root_signature,
        .vertex_shader = &vertex_shader_entry,
        .fragment_shader = &fragment_shader_entry,
        .color_format = &Backend_Format,
        .render_target_count = 1,
        .primitive_topology = GPUPrimitiveTopology::e_triangle_list,
    };

    pipeline = GPU_create_graphics_pipeline(pipeline_desc);

    GPU_destroy_shader_library(vertex_shader);
    GPU_destroy_shader_library(fragment_shader);

    // compute pipeline
    Vector<char> compute_shader = read_file(RES_DIR"shader/post-processing/blur.hlsl");

    Vector<char> cs_compile = compile_shader(compute_shader, L"mean_blur", GPUShaderStageFlag::e_compute);

    GPUShaderLibraryCreateInfo cs_desc{
        .code = reinterpret_cast<uint32_t*>(cs_compile.data()),
        .code_size = static_cast<uint32_t>(cs_compile.size()),
        .stage = GPUShaderStageFlag::e_compute,
    };

    GPUShaderLibrary* cs = GPU_create_shader_library(device, cs_desc);

    GPUShaderEntry compute_shader_entry{
        .library = cs,
        .entry = "mean_blur",
        .stage = GPUShaderStageFlag::e_compute,
    };

    GPURootSignatureCreateInfo compute_rs_desc{
        .shaders = { compute_shader_entry },
        .push_constant_names = {"b_blur_parameter"}
    };

    compute_root_signature = GPU_create_root_signature(device, compute_rs_desc);

    GPUComputePipelineCreateInfo compute_pipeline_desc{
        .root_signature = compute_root_signature,
        .compute_shader = &compute_shader_entry,
    };

    compute_pipeline = GPU_create_compute_pipeline(compute_pipeline_desc);

    GPU_destroy_shader_library(cs);


    GPUTextureCreateInfo rw_texture_create_info{
        .width = Width,
        .height = Height,
        .depth = 1,
        .array_layers = 1,
        .mip_levels = 1,
        .sample_quality = 0,
        .sample_count = GPUSampleCount::e_1,
        .format = Backend_Format,
        .state = GPUResourceStateFlag::e_unordered_access,
        .type = GPUResourceTypeFlag::e_rw_texture,
        .flags = GPUTextureFlagsFlag::e_dedicated
    };
    rw_texture_create_info.type |= GPUResourceTypeFlag::e_texture;
    rw_texture = GPU_create_texture(device, rw_texture_create_info);

    GPUTextureViewCreateInfo rw_texture_view_create_info{
        .texture = rw_texture,
        .format = Backend_Format,
        .usage = GPUTextureViewUsageFlag::e_uav,
        .aspect = GPUTextureViewAspectFlag::e_color,
        .type = GPUTextureType::e_2d,
        .base_array_layer = 0,
        .array_layers = 1,
        .base_mip_level = 0,
        .mip_levels = 1
    };
    rw_texture_view_create_info.usage |= GPUTextureViewUsageFlag::e_srv;
    rw_texture_view = GPU_create_texture_view(rw_texture_view_create_info);

    GPUDescriptorSetCreateInfo descriptor_set_create_info{
        .root_signature = compute_root_signature,
        .set_index = 0
    };
    compute_srv_set = GPU_create_descriptor_set(descriptor_set_create_info);
    second_compute_srv_set = GPU_create_descriptor_set(descriptor_set_create_info);

    GPUDescriptorData descriptor_data{
        .resource_type = GPUResourceTypeFlag::e_texture,
        .binding = 0,
        .textures = &texture_view,
        .array_count = 1
    };
    compute_srv_set->update(&descriptor_data, 1);
    descriptor_data.textures = &rw_texture_view;
    second_compute_srv_set->update(&descriptor_data, 1);

    descriptor_set_create_info.set_index = 1;
    compute_uav_set = GPU_create_descriptor_set(descriptor_set_create_info);
    second_compute_uav_set = GPU_create_descriptor_set(descriptor_set_create_info);

    descriptor_data.resource_type = GPUResourceTypeFlag::e_rw_texture;
    descriptor_data.textures = &rw_texture_view;
    compute_uav_set->update(&descriptor_data, 1);
    descriptor_data.textures = &texture_view;
    second_compute_uav_set->update(&descriptor_data, 1);

    // back pipeline
    Vector<char> back_shader = read_file(RES_DIR"shader/quad/quad.hlsl");

    Vector<char> back_vs_compile = compile_shader(back_shader, L"vs", GPUShaderStageFlag::e_vertex);
    Vector<char> back_fs_compile = compile_shader(back_shader, L"ps", GPUShaderStageFlag::e_fragment);

    GPUShaderLibraryCreateInfo back_vs_desc{
        .code = reinterpret_cast<uint32_t*>(back_vs_compile.data()),
        .code_size = static_cast<uint32_t>(back_vs_compile.size()),
        .stage = GPUShaderStageFlag::e_vertex,
    };
    GPUShaderLibraryCreateInfo back_fs_desc{
        .code = reinterpret_cast<uint32_t*>(back_fs_compile.data()),
        .code_size = static_cast<uint32_t>(back_fs_compile.size()),
        .stage = GPUShaderStageFlag::e_fragment,
    };

    GPUShaderLibrary* back_vertex_shader = GPU_create_shader_library(device, back_vs_desc);

    GPUShaderLibrary* back_fragment_shader = GPU_create_shader_library(device, back_fs_desc);

    GPUShaderEntry back_vertex_shader_entry{
        .library = back_vertex_shader,
        .entry = "vs",
        .stage = GPUShaderStageFlag::e_vertex,
    };

    GPUShaderEntry back_fragment_shader_entry{
        .library = back_fragment_shader,
        .entry = "ps",
        .stage = GPUShaderStageFlag::e_fragment,
    };

    GPURootSignatureCreateInfo back_rs_desc{
        .shaders = { back_vertex_shader_entry, back_fragment_shader_entry },
    };

    GPUSamplerCreateInfo sampler_create_info{
        .min_filter = GPUFilterType::e_linear,
        .mag_filter = GPUFilterType::e_linear,
        .mipmap_mode = GPUMipMapMode::e_linear,
        .address_u = GPUAddressMode::e_repeat,
        .address_v = GPUAddressMode::e_repeat,
        .address_w = GPUAddressMode::e_repeat,
        .compare_mode = GPUCompareMode::e_never,
    };
    sampler = GPU_create_sampler(device, sampler_create_info);
    back_rs_desc.static_samplers.emplace_back("texture_sampler", sampler);

    back_root_signature = GPU_create_root_signature(device, back_rs_desc);

    GPUDescriptorSetCreateInfo back_descriptor_set_create_info{
        .root_signature = back_root_signature,
        .set_index = 0
    };
    back_texture_set = GPU_create_descriptor_set(back_descriptor_set_create_info);

    GPUDescriptorData back_texture_set_data;
    back_texture_set_data.array_count = 1;
    back_texture_set_data.textures = &texture_view;
    back_texture_set_data.resource_type = GPUResourceTypeFlag::e_texture;
    back_texture_set_data.binding = 0;
    back_texture_set->update(&back_texture_set_data, 1);

    GPUGraphicsPipelineCreateInfo back_pipeline_desc{
        .root_signature = back_root_signature,
        .vertex_shader = &back_vertex_shader_entry,
        .fragment_shader = &back_fragment_shader_entry,
        .color_format = &Backend_Format,
        .render_target_count = 1,
        .primitive_topology = GPUPrimitiveTopology::e_triangle_list,
    };

    back_pipeline = GPU_create_graphics_pipeline(back_pipeline_desc);

    GPU_destroy_shader_library(back_vertex_shader);
    GPU_destroy_shader_library(back_fragment_shader);
}

void destroy_pipeline()
{
    GPU_destroy_sampler(sampler);
    GPU_destroy_descriptor_set(back_texture_set);
    GPU_destroy_graphics_pipeline(back_pipeline);
    GPU_destroy_root_signature(back_root_signature);

    GPU_destroy_texture_view(rw_texture_view);
    GPU_destroy_texture(rw_texture);
    GPU_destroy_descriptor_set(second_compute_srv_set);
    GPU_destroy_descriptor_set(second_compute_uav_set);
    GPU_destroy_descriptor_set(compute_srv_set);
    GPU_destroy_descriptor_set(compute_uav_set);
    GPU_destroy_compute_pipeline(compute_pipeline);
    GPU_destroy_root_signature(compute_root_signature);

    GPU_destroy_texture_view(texture_view);
    GPU_destroy_texture(texture);
    GPU_destroy_graphics_pipeline(pipeline);
    GPU_destroy_root_signature(root_signature);
}

void work_thread()
{
    GPUCommandBuffer* command_buffer[Frame_In_Flight] = {};
    GPUCommandBuffer* compute_buffer[Frame_In_Flight] = {};
    for (uint32_t i = 0; i < Frame_In_Flight; i++)
    {
        command_buffer[i] = GPU_create_command_buffer(command_pool_for_work[i], {});
        compute_buffer[i] = GPU_create_command_buffer(compute_pool[i], {});
    }
    GPUSemaphore* graphics_semaphore = GPU_create_semaphore(device);

    while (!quit)
    {
        uint32_t current_index = 0;
        {
            std::unique_lock<std::mutex> lock(mutex);
            while (!work_running)
                condition.wait(lock);
            current_index = index;
        }

        command_pool_for_work[current_index]->reset();
        command_buffer[current_index]->begin_command();

        // graphics
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

        GPUGraphicsPassEncoder* encoder = command_buffer[index]->begin_graphics_pass(graphics_pass_create_info);

        encoder->set_viewport(0, 0, Width, Height, 0, 1);
        encoder->set_scissor(0, 0, Width, Height);
        encoder->bind_pipeline(pipeline);
        encoder->draw(3, 0);

        command_buffer[current_index]->end_graphics_pass(encoder);

        GPUTextureBarrier compute_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_render_target,
            .dst_state = GPUResourceStateFlag::e_non_pixel_shader_resource,
        };
        GPUResourceBarrierInfo srv_barrier{
            .texture_barriers = {compute_barrier}
        };
        command_buffer[current_index]->resource_barrier(srv_barrier);
        command_buffer[current_index]->end_command();

        GPUQueueSubmitInfo submit_info{
            .command_buffers = {command_buffer[current_index]},
            .signal_semaphores = {graphics_semaphore}
        };

        {
            std::unique_lock<std::mutex> lock(submit_mutex);
            graphics_queue->submit(submit_info);
        }

        // compute
        compute_pool[current_index]->reset();
        compute_buffer[current_index]->begin_command();

        GPUComputePassCreateInfo compute_pass_create_info{};
        GPUComputePassEncoder* cs = compute_buffer[current_index]->begin_compute_pass(compute_pass_create_info);
        cs->bind_pipeline(compute_pipeline);
        cs->bind_descriptor_set(compute_srv_set);
        cs->bind_descriptor_set(compute_uav_set);

        int blur[] = {5, 0};
        cs->set_push_constant(compute_root_signature, "b_blur_parameter", blur);
        cs->dispatch(Width / 8, Height / 8, 1);

        // barrier
        GPUTextureBarrier second_srv_barrier{
            .texture = rw_texture,
            .src_state = GPUResourceStateFlag::e_unordered_access,
            .dst_state = GPUResourceStateFlag::e_non_pixel_shader_resource,
        };
        GPUTextureBarrier second_uav_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_non_pixel_shader_resource,
            .dst_state = GPUResourceStateFlag::e_unordered_access,
        };
        GPUResourceBarrierInfo second_barrier_info{
            .texture_barriers = {second_srv_barrier, second_uav_barrier}
        };
        compute_buffer[current_index]->resource_barrier(second_barrier_info);

        cs->bind_descriptor_set(second_compute_srv_set);
        cs->bind_descriptor_set(second_compute_uav_set);
        blur[1] = 1;
        cs->set_push_constant(compute_root_signature, "b_blur_parameter", blur);
        cs->dispatch(Width / 8, Height / 8, 1);

        compute_buffer[current_index]->end_compute_pass(cs);
        compute_buffer[current_index]->end_command();

        GPUQueueSubmitInfo compute_submit_info{
            .command_buffers = {compute_buffer[current_index]},
            .wait_semaphores = {graphics_semaphore},
            .signal_semaphores = {thread_semaphore[current_index]},
        };
        compute_queue->submit(compute_submit_info);

        work_running = false;

        {
            std::unique_lock<std::mutex> lock(mutex);
            present_running = true;
            condition.notify_all();
        }
    }

    {
        std::unique_lock<std::mutex> lock(submit_mutex);
        graphics_queue->wait_idle();
        compute_queue->wait_idle();
    }

    for (uint32_t i = 0; i < Frame_In_Flight; i++)
    {
        GPU_destroy_command_buffer(compute_buffer[i]);
        GPU_destroy_command_buffer(command_buffer[i]);
    }

    GPU_destroy_semaphore(graphics_semaphore);
}

void present_thread()
{
    GPUCommandBuffer* command_buffer[Frame_In_Flight] = {};
    GPUFence* present_fence[Frame_In_Flight]{};
    for (uint32_t i = 0; i < Frame_In_Flight; i++)
    {
        command_buffer[i] = GPU_create_command_buffer(command_pool_for_present[i], {});
        present_fence[i] = GPU_create_fence(device);
    }

    GPUSemaphore* image_semaphore = GPU_create_semaphore(device);
    GPUSemaphore* present_semaphore = GPU_create_semaphore(device);

    while (!quit)
    {
        // draw
        index = swap_chain->acquire_next_frame(image_semaphore, nullptr);
        if (index == std::numeric_limits<uint32_t>::max())
            continue;

        {
            std::unique_lock<std::mutex> lock(mutex);
            work_running = true;
            condition.notify_all();
        }

        GPUTexture const* back_texture = swap_chain->fetch_back_texture(index);
        GPUTextureView const* back_texture_view = swap_chain->fetch_back_texture_view(index);

        {
            std::unique_lock<std::mutex> lock(mutex);
            while (!present_running)
                condition.wait(lock);
        }

        // barrier
        command_pool_for_present[index]->reset();
        command_buffer[index]->begin_command();

        GPUTextureBarrier rw_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_unordered_access,
            .dst_state = GPUResourceStateFlag::e_pixel_shader_resource,
        };
        GPUTextureBarrier back_srv_barrier{
            .texture = back_texture,
            .src_state = GPUResourceStateFlag::e_undefined,
            .dst_state = GPUResourceStateFlag::e_render_target,
        };
        GPUResourceBarrierInfo back_barrier{
            .texture_barriers = {rw_barrier, back_srv_barrier}
        };
        command_buffer[index]->resource_barrier(back_barrier);
        // back
        GPUColorAttachment back_color_attachment{
            .texture_view = back_texture_view,
            .load = GPULoadAction::e_clear,
            .store = GPUStoreAction::e_store,
            .clear_color = {0, 0, 0, 1}
        };

        GPUGraphicsPassCreateInfo back_graphics_pass_create_info{
            .name = "quad",
            .sample_count = GPUSampleCount::e_1,
            .color_attachments = {back_color_attachment},
            .color_attachment_count = 1,
            .depth_stencil_attachment = nullptr,
        };

        GPUGraphicsPassEncoder* back_encoder = command_buffer[index]->begin_graphics_pass(back_graphics_pass_create_info);
        back_encoder->bind_descriptor_set(back_texture_set);
        back_encoder->set_viewport(0, 0, Width, Height, 0, 1);
        back_encoder->set_scissor(0, 0, Width, Height);
        back_encoder->bind_pipeline(back_pipeline);
        back_encoder->draw(3, 0);

        command_buffer[index]->end_graphics_pass(back_encoder);

        GPUTextureBarrier back_present_barrier{
            .texture = back_texture,
            .src_state = GPUResourceStateFlag::e_render_target,
            .dst_state = GPUResourceStateFlag::e_present,
        };
        GPUTextureBarrier t_barrier{
            .texture = texture,
            .src_state = GPUResourceStateFlag::e_non_pixel_shader_resource,
            .dst_state = GPUResourceStateFlag::e_render_target,
        };
        GPUTextureBarrier u_barrier{
            .texture = rw_texture,
            .src_state = GPUResourceStateFlag::e_pixel_shader_resource,
            .dst_state = GPUResourceStateFlag::e_unordered_access,
        };
        GPUResourceBarrierInfo present_barrier{
            .texture_barriers = {back_present_barrier, t_barrier, u_barrier}
        };
        command_buffer[index]->resource_barrier(present_barrier);

        command_buffer[index]->end_command();

        GPUQueueSubmitInfo queue_submit_info{
            .command_buffers = {command_buffer[index]},
            .wait_semaphores = {image_semaphore, thread_semaphore[index]},
            .signal_semaphores = {present_semaphore},
            .signal_fence = present_fence[index],
        };

        GPUQueuePresentInfo queue_present_info{
            .swap_chain = swap_chain,
            .wait_semaphores = {present_semaphore},
            .index = static_cast<uint8_t>(index),
        };

        {
            std::unique_lock<std::mutex> lock(submit_mutex);
            graphics_queue->submit(queue_submit_info);
            present_fence[index]->wait();
            graphics_queue->present(queue_present_info);
        }

        present_running = false;
    }

    work_running = true;
    condition.notify_all();
    {
        std::unique_lock<std::mutex> lock(submit_mutex);
        graphics_queue->wait_idle();
    }

    for (uint32_t i = 0; i < Frame_In_Flight; i++)
    {
        present_fence[i]->wait();
        GPU_destroy_fence(present_fence[i]);
        GPU_destroy_command_buffer(command_buffer[i]);
    }

    GPU_destroy_semaphore(image_semaphore);
    GPU_destroy_semaphore(present_semaphore);
}

void logic_thread()
{
    while (!quit)
    {

    }
}

void draw(SDL_Window* window)
{
    HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
    HINSTANCE hinstance = static_cast<HINSTANCE>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr));

    GPUInstanceCreateInfo instance_create_info{
        .backend = GPUBackend::e_d3d12,
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
    };

    instance = GPU_create_instance(instance_create_info);

    Vector<GPUAdapter*> adapters;
    instance->enum_adapters(adapters);

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

    device = GPU_create_device(adapter, device_create_info);

    graphics_queue = const_cast<GPUQueue*>(device->fetch_queue(GPUQueueType::e_graphics, 0));
    compute_queue = const_cast<GPUQueue*>(device->fetch_queue(GPUQueueType::e_compute, 0));

    GPUCommandBufferCreateInfo command_buffer_create_info{};

    GPUCommandPoolCreateInfo command_pool_create_info{};

    for (uint32_t i = 0; i < Frame_In_Flight; i++)
    {
        command_pool_for_work[i] = GPU_create_command_pool(graphics_queue, command_pool_create_info);
        command_pool_for_present[i] = GPU_create_command_pool(graphics_queue, command_pool_create_info);
        compute_pool[i] = GPU_create_command_pool(compute_queue, command_pool_create_info);
        thread_semaphore[i] = GPU_create_semaphore(device);
    }

    surface = GPU_create_surface(instance, hwnd, hinstance);

    GPUSwapChainCreateInfo swap_chain_create_info{
        .width = Width,
        .height = Height,
        .frame_count = Frame_In_Flight,
        .format = Backend_Format,
        .enable_vsync = true,
        .surface = surface,
        .present_queues = { graphics_queue }
    };

    swap_chain = GPU_create_swap_chain(device, swap_chain_create_info);

    create_pipeline();


    std::thread work(work_thread);
    std::thread present(present_thread);
    std::thread logic(logic_thread);

    quit = false;
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
            default:
                break;
            }
        }
    }

    logic.join();
    work.join();
    present.join();

    destroy_pipeline();

    GPU_destroy_swap_chain(swap_chain);
    GPU_destroy_surface(surface);
    for (uint32_t i = 0; i < Frame_In_Flight; i++)
    {
        GPU_destroy_command_pool(command_pool_for_work[i]);
        GPU_destroy_command_pool(command_pool_for_present[i]);
        GPU_destroy_command_pool(compute_pool[i]);
        GPU_destroy_semaphore(thread_semaphore[i]);
    }

    GPU_destroy_device(device);
    GPU_destroy_instance(instance);
}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return -1;

    SDL_Window* window = SDL_CreateWindow("Amazing Rendering", Width, Height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    draw(window);

    return 0;
}