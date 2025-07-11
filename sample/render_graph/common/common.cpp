//
// Created by AmazingBuff on 2025/7/7.
//

#include "common.h"
#include <Unknwn.h>
#include <dxc/dxcapi.h>
#include <filesystem>

AMAZING_NAMESPACE_BEGIN

RenderBackend Backend = RenderBackend::e_d3d12;
RENDERDOC_API_1_6_0* Renderdoc_Api = nullptr;

void load_render_doc_api()
{
    if (Renderdoc_Api == nullptr)
    {
        if (HMODULE module = GetModuleHandleA("renderdoc.dll"))
        {
            pRENDERDOC_GetAPI api = reinterpret_cast<pRENDERDOC_GetAPI>(GetProcAddress(module, "RENDERDOC_GetAPI"));
            ASSERT(api && api(eRENDERDOC_API_Version_1_6_0, reinterpret_cast<void**>(&Renderdoc_Api)) == 1, "Renderdoc api load", "unable load renderdoc api!");
        }
    }
}

Vector<char> compile_shader(const Vector<char>& code, const wchar_t* entry, RenderShaderStage stage)
{
    const wchar_t* shader_model = nullptr;
    switch (stage)
    {
    case RenderShaderStage::e_vertex:
        shader_model = L"vs_6_0";
        break;
    case RenderShaderStage::e_fragment:
        shader_model = L"ps_6_0";
        break;
    case RenderShaderStage::e_compute:
        shader_model = L"cs_6_0";
        break;
    case RenderShaderStage::e_geometry:
        shader_model = L"gs_6_0";
        break;
    case RenderShaderStage::e_tessellation_control:
        shader_model = L"hs_6_0";
        break;
    case RenderShaderStage::e_tessellation_evaluation:
        shader_model = L"ds_6_0";
        break;
    default:
        shader_model = L"";
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
        L"-I", inc,
        L"-Wno-ignored-attributes"
    };
    if (Backend == RenderBackend::e_vulkan)
        arguments.push_back(L"-spirv");

    IDxcResult* ret = nullptr;
    compiler->Compile(&buffer, arguments.data(), arguments.size(), include_handler, IID_PPV_ARGS(&ret));
    {
        IDxcBlobUtf8* errors = nullptr;
        ret->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
        if (errors != nullptr && errors->GetStringLength() > 0)
            LOG_ERROR("Compile Shader", static_cast<const char*>(errors->GetBufferPointer()));
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


AMAZING_NAMESPACE_END