//
// Created by AmazingBuff on 2025/4/25.
//

#include <rendering/rhi/d3d12/dx12instance.h>
#include <rendering/rhi/d3d12/dx12adapter.h>
#include <rendering/rhi/d3d12/dx12device.h>
#include <rendering/rhi/d3d12/dx12fence.h>
#include <rendering/rhi/d3d12/dx12swapchain.h>
#include <rendering/rhi/d3d12/dx12root_signature.h>
#include <rendering/rhi/d3d12/dx12graphics_pipeline.h>
#include <rendering/rhi/d3d12/resources/dx12shader_library.h>

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

void initialize(HWND hwnd)
{
    GPUInstanceCreateInfo instance_create_info{
        .backend = GPUBackend::e_d3d12,
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
    };

    DX12Instance instance;
    instance.initialize(instance_create_info);

    uint32_t num_adapters = 0;
    instance.enum_adapters(nullptr, &num_adapters);
    DX12Adapter const* adapters = nullptr;
    instance.enum_adapters(reinterpret_cast<GPUAdapter const**>(&adapters), &num_adapters);

    DX12Adapter const& adapter = adapters[0];


    GPUQueueGroup queue_group{
        .queue_type = GPUQueueType::e_graphics,
        .queue_count = 1
    };
    GPUDeviceCreateInfo device_create_info{
        .disable_pipeline_cache = false,
    };

    device_create_info.queue_groups.emplace_back(queue_group);
    DX12Device device;
    device.initialize(&adapter, device_create_info);

    GPUQueue const* queue = device.get_queue(GPUQueueType::e_graphics, 0);

    DX12Fence present_fence;
    present_fence.initialize(&device);

    GPUSurface* surface = reinterpret_cast<GPUSurface*>(hwnd);

    GPUSwapChainCreateInfo swap_chain_create_info{
        .width = 1920,
        .height = 1080,
        .frame_count = 2,
        .format = GPUFormat::e_r8g8b8a8_unorm,
        .enable_vsync = true,
        .surface = surface,
        .present_queues = { queue }
    };

    DX12SwapChain swap_chain;
    swap_chain.initialize(&instance, &device, swap_chain_create_info);


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

    DX12ShaderLibrary vertex_shader;
    vertex_shader.initialize(vs_desc);

    DX12ShaderLibrary fragment_shader;
    fragment_shader.initialize(fs_desc);

    GPUShaderEntry vertex_shader_entry{
        .library = &vertex_shader,
        .entry = "main",
        .stage = GPUShaderStageFlag::e_vertex,
    };

    GPUShaderEntry fragment_shader_entry{
        .library = &fragment_shader,
        .entry = "main",
        .stage = GPUShaderStageFlag::e_fragment,
    };

    GPURootSignatureCreateInfo rs_desc{
        .shaders = { vertex_shader_entry, fragment_shader_entry },
    };

    DX12RootSignature root_sig;
    root_sig.initialize(&device, rs_desc);

    GPUFormat backend_format = GPUFormat::e_r8g8b8a8_unorm;

    GPUGraphicsPipelineCreateInfo pipeline_desc{
        .root_signature = &root_sig,
        .vertex_shader = &vertex_shader_entry,
        .fragment_shader = &fragment_shader_entry,
        .color_format = &backend_format,
        .render_target_count = 1,
        .primitive_topology = GPUPrimitiveTopology::e_triangle_list,
    };

    DX12GraphicsPipeline pipeline;
    pipeline.initialize(&device, pipeline_desc);
}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return -1;

    SDL_Window* window = SDL_CreateWindow("Amazing Rendering", 1920, 1080, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

    initialize(hwnd);

    return 0;
}