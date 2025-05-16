//
// Created by AmazingBuff on 2025/5/2.
//

#include <common.h>

thread_local GPURootSignature* root_signature = nullptr;
thread_local GPUGraphicsPipeline* pipeline = nullptr;
thread_local GPUDescriptorSet* texture_set = nullptr;
thread_local GPUDescriptorSet* buffer_set = nullptr;
thread_local GPUDescriptorSet* sampler_set = nullptr;
thread_local GPUTexture* texture = nullptr;
thread_local GPUTextureView* texture_view = nullptr;
thread_local GPUSampler* sampler = nullptr;
thread_local GPUBuffer* buffer = nullptr;
thread_local GPUBuffer* vertex_buffer = nullptr;
thread_local GPUBuffer* index_buffer = nullptr;

thread_local bool use_static_samplers = true;


struct ObjectInfo
{
    Affine3f model;
    Affine3f model_inv;
};
thread_local ObjectInfo world;

struct PassInfo
{
    Affine3f view;
    Affine3f proj;
};
thread_local PassInfo push_constant;

constexpr static Float vertices[] = {
    // position       // texcoord // normal          // tangent
    -1.0, -1.0, -1.0, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    -1.0, +1.0, -1.0, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    +1.0, +1.0, -1.0, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    +1.0, -1.0, -1.0, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    -1.0, -1.0, +1.0, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    +1.0, -1.0, +1.0, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    +1.0, +1.0, +1.0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0, +1.0, +1.0, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0, +1.0, -1.0, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -1.0, +1.0, +1.0, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    +1.0, +1.0, +1.0, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    +1.0, +1.0, -1.0, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -1.0, -1.0, -1.0, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    +1.0, -1.0, -1.0, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    +1.0, -1.0, +1.0, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -1.0, -1.0, +1.0, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -1.0, -1.0, +1.0, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -1.0, +1.0, +1.0, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -1.0, +1.0, -1.0, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -1.0, -1.0, -1.0, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    +1.0, -1.0, -1.0, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    +1.0, +1.0, -1.0, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    +1.0, +1.0, +1.0, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    +1.0, -1.0, +1.0, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

constexpr static uint32_t indices[] = {
    0, 1, 2,  0, 2, 3,
    4, 5, 6,  4, 6, 7,
    8, 9, 10, 8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
};

void set_scene()
{
    const Vec3f position{ 3.0, 0.0, 0.0 };
    const Vec3f center{ 0.0, 0.0, 0.0 };
    const Vec3f world_up{ 0.0, 0.0, 1.0 };
    const Vec3f right{ 1.0, 0.0, 0.0 };
    const Vec3f front{ 0.0, 0.0, 1.0 };
    const Vec3f up{ 0.0, 1.0, 0.0 };
    const Float fov = 45.0;
    const Float z_near = 0.1;
    const Float z_far = 10.0;

    Mat3f r;
    r.row(0) = right;
    r.row(1) = up;
    r.row(2) = front;

    Affine3f look(Affine3f::Identity());
    look.linear() = r;
    look.translation() = -look.linear() * position;

    const Float aspect = static_cast<Float>(Width) / Height;

    Float theta = fov * 0.5;
    Float range = z_far - z_near;
    Float inv_tan = 1.0 / std::tan(theta);

    Affine3f proj(Affine3f::Identity());
    proj(0, 0) = inv_tan / aspect;
    proj(1, 1) = inv_tan;
    proj(2, 2) = -(z_far + z_near) / range;
    proj(2, 3) = -2.0 * z_near * z_far / range;
    proj(3, 2) = -1.0;
    proj(3, 3) = 0.0;

    push_constant.view = look;
    push_constant.proj = proj;


    Affine3f model(Affine3f::Identity());
    model.scale(Vec3f(1.0, 1.0, 1.0));
    model.rotate(Eigen::AngleAxisf(180, Vec3f::UnitZ()));
    model.translate(Vec3f(3.0, 0.0, -6.0));

    world.model = model;
    world.model_inv = model.inverse();

    GPUBufferCreateInfo buffer_ci{
    .size = sizeof(vertices),
    .usage = GPUMemoryUsage::e_cpu_to_gpu,
    .flags = GPUBufferFlagsFlag::e_persistent_map,
    };
    vertex_buffer = GPU_create_buffer(t_device, buffer_ci);
    vertex_buffer->map(0, sizeof(vertices), vertices);

    buffer_ci.size = sizeof(indices);
    index_buffer = GPU_create_buffer(t_device, buffer_ci);
    index_buffer->map(0, sizeof(indices), indices);
}

void create_pipeline()
{
    set_scene();
    ImageInfo image = load_image(RES_DIR"image/a.png");

    GPUTextureCreateInfo texture_create_info{
        .width = static_cast<uint32_t>(image.width),
        .height = static_cast<uint32_t>(image.height),
        .depth = 1,
        .array_layers = 1,
        .mip_levels = 1,
        .sample_quality = 0,
        .sample_count = GPUSampleCount::e_1,
        .format = GPUFormat::e_r8g8b8a8_unorm,
        .state = GPUResourceStateFlag::e_copy_destination,
        .type = GPUResourceTypeFlag::e_texture,
        .flags = GPUTextureFlagsFlag::e_dedicated,
    };
    texture = GPU_create_texture(t_device, texture_create_info);

    GPUTextureViewCreateInfo texture_view_create_info{
        .texture = texture,
        .format = GPUFormat::e_r8g8b8a8_unorm,
        .usage = GPUTextureViewUsageFlag::e_srv,
        .aspect = GPUTextureViewAspectFlag::e_color,
        .type = GPUTextureType::e_2d,
        .base_array_layer = 0,
        .array_layers = 1,
        .base_mip_level = 0,
        .mip_levels = 1,
    };
    texture_view = GPU_create_texture_view(texture_view_create_info);

    GPUBufferCreateInfo buffer_create_info{
        .size = image.data.size(),
        .usage = GPUMemoryUsage::e_cpu_to_gpu,
        .flags = GPUBufferFlagsFlag::e_persistent_map,
    };
    GPUBuffer* transfer_buffer = GPU_create_buffer(t_device, buffer_create_info);
    transfer_buffer->map(0, image.data.size(), image.data.data());

    GPUBufferToTextureTransferInfo transfer_info{
        .src_buffer = transfer_buffer,
        .src_buffer_offset = 0,
        .dst_texture = texture,
        .dst_texture_subresource{
            .mip_level = 0,
            .base_array_layer = 0,
            .array_layers = 1
        },
    };
    transfer_buffer_to_texture(transfer_info);

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

    GPUBufferCreateInfo buffer_ci{
        .size = sizeof(ObjectInfo),
        .usage = GPUMemoryUsage::e_cpu_to_gpu,
        .type = GPUResourceTypeFlag::e_uniform_buffer,
        .flags = GPUBufferFlagsFlag::e_persistent_map,
    };
    buffer = GPU_create_buffer(t_device, buffer_ci);
    buffer->map(0, sizeof(ObjectInfo), &world);

    // graphics pipeline
    Vector<char> vs = read_file(RES_DIR"shader/box/box.hlsl");
    Vector<char> fs = read_file(RES_DIR"shader/box/box.hlsl");

    Vector<char> vs_compile = compile_shader(vs, L"vs", GPUShaderStageFlag::e_vertex);
    Vector<char> fs_compile = compile_shader(fs, L"ps", GPUShaderStageFlag::e_fragment);

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
        .push_constant_names = { "b_push_constant" },
    };

    if (use_static_samplers)
        rs_desc.static_samplers.emplace_back("s_sampler", sampler);

    root_signature = GPU_create_root_signature(t_device, rs_desc);

    GPUDescriptorSetCreateInfo descriptor_set_create_info{
        .root_signature = root_signature,
        .set_index = 1
    };
    texture_set = GPU_create_descriptor_set(descriptor_set_create_info);

    GPUDescriptorData texture_set_data;
    texture_set_data.array_count = 1;
    texture_set_data.textures = &texture_view;
    texture_set_data.resource_type = GPUResourceTypeFlag::e_texture;
    texture_set_data.binding = 0;
    texture_set->update(&texture_set_data, 1);

    descriptor_set_create_info.set_index = 0;
    buffer_set = GPU_create_descriptor_set(descriptor_set_create_info);

    GPUDescriptorData buffer_set_data;
    buffer_set_data.array_count = 1;
    buffer_set_data.buffers = &buffer;
    buffer_set_data.resource_type = GPUResourceTypeFlag::e_uniform_buffer;
    buffer_set_data.binding = 1;
    buffer_set->update(&buffer_set_data, 1);

    if (!use_static_samplers)
    {
        descriptor_set_create_info.set_index = 2;
        sampler_set = GPU_create_descriptor_set(descriptor_set_create_info);

        GPUDescriptorData sampler_set_data;
        sampler_set_data.array_count = 1;
        sampler_set_data.samplers = &sampler;
        sampler_set_data.resource_type = GPUResourceTypeFlag::e_sampler;
        sampler_set_data.binding = 0;
        sampler_set->update(&sampler_set_data, 1);
    }

    GPUFormat backend_format = GPUFormat::e_r8g8b8a8_unorm;

    GPUVertexAttribute pos{
        .array_size = 1,
        .format = GPUFormat::e_r32g32b32_sfloat,
        .slot = 0,
        .semantic_name = "POSITION",
        .offset = 0,
    };

    GPUVertexAttribute tex{
        .array_size = 1,
        .format = GPUFormat::e_r32g32_sfloat,
        .slot = 0,
        .semantic_name = "TEXCOORD",
        .offset = 12,
    };

    GPUVertexAttribute normal{
        .array_size = 1,
        .format = GPUFormat::e_r32g32b32_sfloat,
        .slot = 0,
        .semantic_name = "NORMAL",
        .offset = 20,
        .rate = GPUVertexInputRate::e_vertex
    };

    GPUVertexAttribute tangent{
        .array_size = 1,
        .format = GPUFormat::e_r32g32b32_sfloat,
        .slot = 0,
        .semantic_name = "TANGENT",
        .offset = 32,
    };

    GPUGraphicsPipelineCreateInfo pipeline_desc{
        .root_signature = root_signature,
        .vertex_shader = &vertex_shader_entry,
        .fragment_shader = &fragment_shader_entry,
        .vertex_inputs = { pos, tex, normal, tangent },
        .color_format = &backend_format,
        .render_target_count = 1,
        .primitive_topology = GPUPrimitiveTopology::e_triangle_list,
    };

    pipeline = GPU_create_graphics_pipeline(pipeline_desc);

    GPU_destroy_shader_library(vertex_shader);
    GPU_destroy_shader_library(fragment_shader);

    GPU_destroy_buffer(transfer_buffer);
}

void destroy_pipeline()
{
    GPU_destroy_graphics_pipeline(pipeline);

    GPU_destroy_buffer(buffer);
    GPU_destroy_buffer(vertex_buffer);
    GPU_destroy_buffer(index_buffer);
    GPU_destroy_descriptor_set(texture_set);
    GPU_destroy_descriptor_set(buffer_set);
    if (!use_static_samplers)
        GPU_destroy_descriptor_set(sampler_set);

    GPU_destroy_root_signature(root_signature);

    GPU_destroy_texture_view(texture_view);
    GPU_destroy_texture(texture);
    GPU_destroy_sampler(sampler);
}

void draw(SDL_Window* window)
{
    HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

    create_api_object(hwnd, GPUBackend::e_d3d12);
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

        GPUBufferBinding binding{
            .buffer = vertex_buffer,
            .stride = 44,
            .offset = 0,
        };
        encoder->bind_vertex_buffers(&binding, 1);
        binding.buffer = index_buffer;
        binding.stride = 3;
        encoder->bind_index_buffer(binding);


        encoder->set_push_constant(root_signature, "b_push_constant", &push_constant);
        encoder->bind_descriptor_set(buffer_set);
        encoder->bind_descriptor_set(texture_set);
        if (!use_static_samplers)
            encoder->bind_descriptor_set(sampler_set);
        encoder->set_viewport(0, 0, Width, Height, 0, 1);
        encoder->set_scissor(0, 0, Width, Height);
        encoder->bind_pipeline(pipeline);
        encoder->draw_indexed(36, 0, 0);

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

    int* p = Allocator<int>::allocate(1);

    return 0;
}