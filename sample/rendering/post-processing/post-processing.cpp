//
// Created by AmazingBuff on 2025/5/29.
//


#include <common.h>

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
        .state = GPUResourceState::e_render_target,
        .type = GPUResourceType::e_render_target | GPUResourceType::e_texture | GPUResourceType::e_rw_texture,
        .flags = GPUTextureFlag::e_dedicated
    };
    texture = GPU_create_texture(t_device, texture_create_info);

    GPUTextureViewCreateInfo texture_view_create_info{
        .texture = texture,
        .format = Backend_Format,
        .usage = GPUTextureViewUsage::e_rtv_dsv | GPUTextureViewUsage::e_srv | GPUTextureViewUsage::e_uav,
        .aspect = GPUTextureViewAspect::e_color,
        .type = GPUTextureType::e_2d,
        .base_array_layer = 0,
        .array_layers = 1,
        .base_mip_level = 0,
        .mip_levels = 1
    };
    texture_view = GPU_create_texture_view(texture_view_create_info);

    Vector<char> shader = read_file(RES_DIR"shader/triangle/triangle.hlsl");

    Vector<char> vs_compile = compile_shader(shader, L"vs", GPUShaderStage::e_vertex);
    Vector<char> fs_compile = compile_shader(shader, L"ps", GPUShaderStage::e_fragment);

    GPUShaderLibraryCreateInfo vs_desc{
        .code = reinterpret_cast<uint32_t*>(vs_compile.data()),
        .code_size = static_cast<uint32_t>(vs_compile.size()),
        .stage = GPUShaderStage::e_vertex,
    };
    GPUShaderLibraryCreateInfo fs_desc{
        .code = reinterpret_cast<uint32_t*>(fs_compile.data()),
        .code_size = static_cast<uint32_t>(fs_compile.size()),
        .stage = GPUShaderStage::e_fragment,
    };

    GPUShaderLibrary* vertex_shader = GPU_create_shader_library(t_device, vs_desc);

    GPUShaderLibrary* fragment_shader = GPU_create_shader_library(t_device, fs_desc);

    GPUShaderEntry vertex_shader_entry{
        .library = vertex_shader,
        .entry = "vs",
        .stage = GPUShaderStage::e_vertex,
    };

    GPUShaderEntry fragment_shader_entry{
        .library = fragment_shader,
        .entry = "ps",
        .stage = GPUShaderStage::e_fragment,
    };

    GPURootSignatureCreateInfo rs_desc{
        .shaders = { vertex_shader_entry, fragment_shader_entry },
    };

    root_signature = GPU_create_root_signature(t_device, rs_desc);

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

    Vector<char> cs_compile = compile_shader(compute_shader, L"mean_blur", GPUShaderStage::e_compute);

    GPUShaderLibraryCreateInfo cs_desc{
        .code = reinterpret_cast<uint32_t*>(cs_compile.data()),
        .code_size = static_cast<uint32_t>(cs_compile.size()),
        .stage = GPUShaderStage::e_compute,
    };

    GPUShaderLibrary* cs = GPU_create_shader_library(t_device, cs_desc);

    GPUShaderEntry compute_shader_entry{
        .library = cs,
        .entry = "mean_blur",
        .stage = GPUShaderStage::e_compute,
    };

    GPURootSignatureCreateInfo compute_rs_desc{
        .shaders = { compute_shader_entry },
        .push_constant_names = {"b_blur_parameter"}
    };

    compute_root_signature = GPU_create_root_signature(t_device, compute_rs_desc);

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
        .state = GPUResourceState::e_unordered_access,
        .type = GPUResourceType::e_rw_texture | GPUResourceType::e_texture,
        .flags = GPUTextureFlag::e_dedicated
    };
    rw_texture = GPU_create_texture(t_device, rw_texture_create_info);

    GPUTextureViewCreateInfo rw_texture_view_create_info{
        .texture = rw_texture,
        .format = Backend_Format,
        .usage = GPUTextureViewUsage::e_uav | GPUTextureViewUsage::e_srv,
        .aspect = GPUTextureViewAspect::e_color,
        .type = GPUTextureType::e_2d,
        .base_array_layer = 0,
        .array_layers = 1,
        .base_mip_level = 0,
        .mip_levels = 1
    };
    rw_texture_view = GPU_create_texture_view(rw_texture_view_create_info);

    GPUDescriptorSetCreateInfo descriptor_set_create_info{
        .root_signature = compute_root_signature,
        .set_index = 0
    };
    compute_srv_set = GPU_create_descriptor_set(descriptor_set_create_info);
    second_compute_srv_set = GPU_create_descriptor_set(descriptor_set_create_info);

    GPUDescriptorData descriptor_data{
        .resource_type = GPUResourceType::e_texture,
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

    descriptor_data.resource_type = GPUResourceType::e_rw_texture;
    descriptor_data.textures = &rw_texture_view;
    compute_uav_set->update(&descriptor_data, 1);
    descriptor_data.textures = &texture_view;
    second_compute_uav_set->update(&descriptor_data, 1);

    // back pipeline
    Vector<char> back_shader = read_file(RES_DIR"shader/quad/quad.hlsl");

    Vector<char> back_vs_compile = compile_shader(back_shader, L"vs", GPUShaderStage::e_vertex);
    Vector<char> back_fs_compile = compile_shader(back_shader, L"ps", GPUShaderStage::e_fragment);

    GPUShaderLibraryCreateInfo back_vs_desc{
        .code = reinterpret_cast<uint32_t*>(back_vs_compile.data()),
        .code_size = static_cast<uint32_t>(back_vs_compile.size()),
        .stage = GPUShaderStage::e_vertex,
    };
    GPUShaderLibraryCreateInfo back_fs_desc{
        .code = reinterpret_cast<uint32_t*>(back_fs_compile.data()),
        .code_size = static_cast<uint32_t>(back_fs_compile.size()),
        .stage = GPUShaderStage::e_fragment,
    };

    GPUShaderLibrary* back_vertex_shader = GPU_create_shader_library(t_device, back_vs_desc);

    GPUShaderLibrary* back_fragment_shader = GPU_create_shader_library(t_device, back_fs_desc);

    GPUShaderEntry back_vertex_shader_entry{
        .library = back_vertex_shader,
        .entry = "vs",
        .stage = GPUShaderStage::e_vertex,
    };

    GPUShaderEntry back_fragment_shader_entry{
        .library = back_fragment_shader,
        .entry = "ps",
        .stage = GPUShaderStage::e_fragment,
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
    sampler = GPU_create_sampler(t_device, sampler_create_info);
    back_rs_desc.static_samplers.emplace_back("texture_sampler", sampler);

    back_root_signature = GPU_create_root_signature(t_device, back_rs_desc);

    GPUDescriptorSetCreateInfo back_descriptor_set_create_info{
        .root_signature = back_root_signature,
        .set_index = 0
    };
    back_texture_set = GPU_create_descriptor_set(back_descriptor_set_create_info);

    GPUDescriptorData back_texture_set_data;
    back_texture_set_data.array_count = 1;
    back_texture_set_data.textures = &texture_view;
    back_texture_set_data.resource_type = GPUResourceType::e_texture;
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

void draw(SDL_Window* window)
{
    HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
    HINSTANCE hinstance = static_cast<HINSTANCE>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr));

    create_api_object(hwnd, hinstance, GPUBackend::e_vulkan);
    create_pipeline();

    GPUSemaphore* graphic_semaphore = GPU_create_semaphore(t_device);
    GPUSemaphore* compute_semaphore = GPU_create_semaphore(t_device);

    GPUCommandBuffer* command_buffer[Frame_In_Flight] = {};
    for (uint32_t i = 0; i < array_size(command_buffer); i++)
    {
        command_buffer[i] = GPU_create_command_buffer(t_command_pool[i], {});
    }

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
            default:
                break;
            }
        }

        // draw
        uint32_t index = t_swap_chain->acquire_next_frame(t_image_semaphore, nullptr);
        if (index == std::numeric_limits<uint32_t>::max())
            continue;

        GPUTexture const* back_texture = t_swap_chain->fetch_back_texture(index);
        GPUTextureView const* back_texture_view = t_swap_chain->fetch_back_texture_view(index);

        t_command_pool[index]->reset();
        t_command_buffer[index]->begin_command();

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

        GPUGraphicsPassEncoder* encoder = t_command_buffer[index]->begin_graphics_pass(graphics_pass_create_info);

        encoder->set_viewport(0, 0, Width, Height, 0, 1);
        encoder->set_scissor(0, 0, Width, Height);
        encoder->bind_pipeline(pipeline);
        encoder->draw(3, 0);

        t_command_buffer[index]->end_graphics_pass(encoder);

        GPUTextureBarrier compute_barrier{
            .texture = texture,
            .src_state = GPUResourceState::e_render_target,
            .dst_state = GPUResourceState::e_non_pixel_shader_resource,
        };
        GPUResourceBarrierInfo srv_barrier{
            .texture_barriers = {compute_barrier}
        };
        t_command_buffer[index]->resource_barrier(srv_barrier);
        t_command_buffer[index]->end_command();

        GPUQueueSubmitInfo submit_info{
            .command_buffers = {t_command_buffer[index]},
            .signal_semaphores = {graphic_semaphore}
        };
        t_graphics_queue->submit(submit_info);

        // compute
        t_compute_pool[index]->reset();
        t_compute_buffer[index]->begin_command();

        GPUComputePassCreateInfo compute_pass_create_info{};
        GPUComputePassEncoder* cs = t_compute_buffer[index]->begin_compute_pass(compute_pass_create_info);
        cs->bind_pipeline(compute_pipeline);
        cs->bind_descriptor_set(compute_srv_set);
        cs->bind_descriptor_set(compute_uav_set);

        int blur[] = {5, 0};
        cs->set_push_constant(compute_root_signature, "b_blur_parameter", blur);
        cs->dispatch(Width / 8, Height / 8, 1);

        // barrier
        GPUTextureBarrier second_srv_barrier{
            .texture = rw_texture,
            .src_state = GPUResourceState::e_unordered_access,
            .dst_state = GPUResourceState::e_non_pixel_shader_resource,
        };
        GPUTextureBarrier second_uav_barrier{
            .texture = texture,
            .src_state = GPUResourceState::e_non_pixel_shader_resource,
            .dst_state = GPUResourceState::e_unordered_access,
        };
        GPUResourceBarrierInfo second_barrier_info{
            .texture_barriers = {second_srv_barrier, second_uav_barrier}
        };
        t_compute_buffer[index]->resource_barrier(second_barrier_info);

        cs->bind_descriptor_set(second_compute_srv_set);
        cs->bind_descriptor_set(second_compute_uav_set);
        blur[1] = 1;
        cs->set_push_constant(compute_root_signature, "b_blur_parameter", blur);
        cs->dispatch(Width / 8, Height / 8, 1);

        t_compute_buffer[index]->end_compute_pass(cs);
        t_compute_buffer[index]->end_command();

        GPUQueueSubmitInfo compute_submit_info{
            .command_buffers = {t_compute_buffer[index]},
            .wait_semaphores = {graphic_semaphore},
            .signal_semaphores = {compute_semaphore},
        };
        t_compute_queue->submit(compute_submit_info);

        // barrier
        command_buffer[index]->begin_command();

        GPUTextureBarrier rw_barrier{
            .texture = texture,
            .src_state = GPUResourceState::e_unordered_access,
            .dst_state = GPUResourceState::e_pixel_shader_resource,
        };
        GPUTextureBarrier back_srv_barrier{
            .texture = back_texture,
            .src_state = GPUResourceState::e_undefined,
            .dst_state = GPUResourceState::e_render_target,
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
            .src_state = GPUResourceState::e_render_target,
            .dst_state = GPUResourceState::e_present,
        };
        GPUTextureBarrier t_barrier{
            .texture = texture,
            .src_state = GPUResourceState::e_pixel_shader_resource,
            .dst_state = GPUResourceState::e_render_target,
        };
        GPUTextureBarrier u_barrier{
            .texture = rw_texture,
            .src_state = GPUResourceState::e_non_pixel_shader_resource,
            .dst_state = GPUResourceState::e_unordered_access,
        };
        GPUResourceBarrierInfo present_barrier{
            .texture_barriers = {back_present_barrier, t_barrier, u_barrier}
        };
        command_buffer[index]->resource_barrier(present_barrier);

        command_buffer[index]->end_command();

        GPUQueueSubmitInfo queue_submit_info{
            .command_buffers = {command_buffer[index]},
            .wait_semaphores = {t_image_semaphore, compute_semaphore},
            .signal_semaphores = {t_present_semaphore},
            .signal_fence = t_present_fence[index],
        };
        t_graphics_queue->submit(queue_submit_info);

        GPUQueuePresentInfo queue_present_info{
            .swap_chain = t_swap_chain,
            .wait_semaphores = {t_present_semaphore},
            .index = static_cast<uint8_t>(index),
        };
        t_graphics_queue->present(queue_present_info);

        t_present_fence[index]->wait();
    }

    t_graphics_queue->wait_idle();
    for (uint32_t i = 0; i < Frame_In_Flight; i++)
        t_present_fence[i]->wait();

    for (uint32_t i = 0; i < Frame_In_Flight; i++)
        GPU_destroy_command_buffer(command_buffer[i]);
    GPU_destroy_semaphore(graphic_semaphore);
    GPU_destroy_semaphore(compute_semaphore);

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