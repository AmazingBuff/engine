//
// Created by AmazingBuff on 2025/4/30.
//

#include "common.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <dxc/dxcapi.h>

#include <filesystem>

thread_local GPUInstance* t_instance = nullptr;
thread_local GPUDevice* t_device = nullptr;
thread_local GPUSurface* t_surface = nullptr;
thread_local GPUQueue* t_graphics_queue = nullptr;
thread_local GPUCommandPool* t_command_pool[Frame_In_Flight] = { nullptr };
thread_local GPUCommandBuffer* t_command_buffer[Frame_In_Flight] = { nullptr };
thread_local GPUSwapChain* t_swap_chain = nullptr;
thread_local GPUFence* t_present_fence[Frame_In_Flight] = { nullptr };
thread_local GPUSemaphore* t_image_semaphore = nullptr;
thread_local GPUSemaphore* t_present_semaphore = nullptr;


thread_local GPUQueue* t_transfer_queue = nullptr;
thread_local GPUCommandPool* t_transfer_pool = nullptr;
thread_local GPUCommandBuffer* t_transfer_buffer = nullptr;

void create_api_object(HWND hwnd, GPUBackend backend)
{
    GPUInstanceCreateInfo instance_create_info{
        .backend = backend,
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
    };

    t_instance = GPU_create_instance(instance_create_info);

    Vector<GPUAdapter*> adapters;
    t_instance->enum_adapters(adapters);

    GPUAdapter* const adapter = adapters[0];

    GPUQueueGroup graphics_queue_group{
        .queue_type = GPUQueueType::e_graphics,
        .queue_count = 1
    };
    GPUQueueGroup transfer_queue_group{
        .queue_type = GPUQueueType::e_transfer,
        .queue_count = 1
    };
    GPUDeviceCreateInfo device_create_info{
        .disable_pipeline_cache = false,
        .queue_groups = {graphics_queue_group, transfer_queue_group},
    };

    t_device = GPU_create_device(adapter, device_create_info);

    t_graphics_queue = (GPUQueue*)t_device->fetch_queue(GPUQueueType::e_graphics, 0);

    t_transfer_queue = (GPUQueue*)t_device->fetch_queue(GPUQueueType::e_transfer, 0);

    GPUCommandBufferCreateInfo command_buffer_create_info{};

    t_transfer_pool = GPU_create_command_pool(t_device, t_transfer_queue);
    t_transfer_buffer = GPU_create_command_buffer(t_device, t_transfer_pool, command_buffer_create_info);

    t_image_semaphore = GPU_create_semaphore(t_device);
    t_present_semaphore = GPU_create_semaphore(t_device);

    for (uint32_t i = 0; i < Frame_In_Flight; i++)
    {
        t_command_pool[i] = GPU_create_command_pool(t_device, t_graphics_queue);
        t_command_buffer[i] = GPU_create_command_buffer(t_device, t_command_pool[i], command_buffer_create_info);
        t_present_fence[i] = GPU_create_fence(t_device);
    }

    t_surface = GPU_create_surface(t_instance, hwnd);

    GPUSwapChainCreateInfo swap_chain_create_info{
        .width = Width,
        .height = Height,
        .frame_count = Frame_In_Flight,
        .format = GPUFormat::e_r8g8b8a8_unorm,
        .enable_vsync = true,
        .surface = t_surface,
        .present_queues = { t_graphics_queue }
    };

    t_swap_chain = GPU_create_swap_chain(t_instance, t_device, swap_chain_create_info);
}

void destroy_api_object()
{
    GPU_destroy_command_buffer(t_transfer_buffer);
    GPU_destroy_command_pool(t_transfer_pool);

    GPU_destroy_semaphore(t_image_semaphore);
    GPU_destroy_semaphore(t_present_semaphore);

    for (uint32_t i = 0; i < Frame_In_Flight; i++)
    {
        GPU_destroy_fence(t_present_fence[i]);
        GPU_destroy_command_buffer(t_command_buffer[i]);
        GPU_destroy_command_pool(t_command_pool[i]);
    }

    GPU_destroy_swap_chain(t_swap_chain);

    GPU_destroy_surface(t_surface);
    GPU_destroy_device(t_device);
    GPU_destroy_instance(t_instance);
}


ImageInfo load_image(const String& file_path)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t* image = stbi_load(file_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    Vector<uint8_t> image_data(width * height * 4);
    memcpy(image_data.data(), image, width * height * 4);

    stbi_image_free(image);
    return { width, height, 4, image_data };
}

Vector<char> compile_shader(const Vector<char>& code, const wchar_t* entry, GPUShaderStageFlag stage, bool spv)
{
    const wchar_t* shader_model = nullptr;
    switch (stage)
    {
    case GPUShaderStageFlag::e_vertex:
        shader_model = L"vs_6_0";
        break;
    case GPUShaderStageFlag::e_fragment:
        shader_model = L"ps_6_0";
        break;
    case GPUShaderStageFlag::e_compute:
        shader_model = L"cs_6_0";
        break;
    case GPUShaderStageFlag::e_geometry:
        shader_model = L"gs_6_0";
        break;
    case GPUShaderStageFlag::e_tessellation_control:
        shader_model = L"hs_6_0";
        break;
    case GPUShaderStageFlag::e_tessellation_evaluation:
        shader_model = L"ds_6_0";
        break;
    }

    IDxcUtils* utils = nullptr;
    IDxcCompiler3* compiler = nullptr;
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

    IDxcIncludeHandler* include_handler = nullptr;
    utils->CreateDefaultIncludeHandler(&include_handler);

    IDxcBlobEncoding* source_blob = nullptr;
    utils->CreateBlob(code.data(), code.size(), DXC_CP_ACP, &source_blob);

    DxcBuffer buffer = {
        .Ptr = source_blob->GetBufferPointer(),
        .Size = source_blob->GetBufferSize(),
        .Encoding = CP_UTF8
    };

    std::filesystem::path include_path(RES_DIR"shader");
    const wchar_t* inc = include_path.c_str();
    Vector<LPCWSTR> arguments = {
        L"-E", entry,
        L"-T", shader_model,
        L"-I", inc
    };
    if (spv)
        arguments.push_back(L"-spirv");

    IDxcResult* ret = nullptr;
    compiler->Compile(&buffer, arguments.data(), arguments.size(), include_handler, IID_PPV_ARGS(&ret));
    {
        IDxcBlobUtf8* errors = nullptr;
        ret->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
        if (errors != nullptr && errors->GetStringLength() > 0)
            LOG_ERROR("Compile Shader", reinterpret_cast<const char*>(errors->GetBufferPointer()));
        errors->Release();
    }

    IDxcBlob* shader = nullptr;
    ret->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader), nullptr);

    Vector<char> res(shader->GetBufferSize());
    memcpy(res.data(), shader->GetBufferPointer(), shader->GetBufferSize());

    ret->Release();
    source_blob->Release();
    include_handler->Release();
    compiler->Release();
    utils->Release();

    return res;
}

void transfer_buffer_to_texture(GPUBufferToTextureTransferInfo const& info)
{
    t_command_buffer[0]->begin_command();
    t_command_buffer[0]->transfer_buffer_to_texture(info);
    GPUTextureBarrier barrier{
        .texture = info.dst_texture,
        .src_state = GPUResourceStateFlag::e_copy_destination,
        .dst_state = GPUResourceStateFlag::e_shader_resource
    };

    GPUResourceBarrierInfo barrier_info{
        .texture_barriers = {barrier}
    };

    t_command_buffer[0]->resource_barrier(barrier_info);
    t_command_buffer[0]->end_command();

    GPUQueueSubmitInfo submit_info{
        .command_buffers = {t_command_buffer[0]},
    };

    t_graphics_queue->submit(submit_info);
    t_graphics_queue->wait_idle();
}