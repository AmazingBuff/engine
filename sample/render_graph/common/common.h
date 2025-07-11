//
// Created by AmazingBuff on 2025/7/8.
//

#ifndef COMMON_H
#define COMMON_H

#include <rendering/render_api.h>
#include <SDL3/SDL.h>
#include <geometry/geometry.h>
#include <renderdoc/renderdoc_app.h>

#ifdef _WIN64
#include <windows.h>
#endif
AMAZING_NAMESPACE_BEGIN

extern RenderBackend Backend;
extern RENDERDOC_API_1_6_0* Renderdoc_Api;

static constexpr uint32_t Width = 960;
static constexpr uint32_t Height = 540;

static constexpr RenderFormat Backend_Format = RenderFormat::e_r8g8b8a8;
static constexpr RenderFormat Backend_Depth_Format = RenderFormat::e_d24s8;

void load_render_doc_api();
Vector<char> compile_shader(const Vector<char>& code, const wchar_t* entry, RenderShaderStage stage);


AMAZING_NAMESPACE_END
#endif //COMMON_H
