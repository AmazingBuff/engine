//
// Created by AmazingBuff on 2025/5/6.
//

#include "vk_utils.h"
#include "rendering/rhi/common/adapter.h"

AMAZING_NAMESPACE_BEGIN

const char* transfer_result(VkResult result)
{
    switch (result)
    {
#define SWITCH_BRANCH(expr) case expr : return #expr
        SWITCH_BRANCH(VK_SUCCESS);
        SWITCH_BRANCH(VK_NOT_READY);
        SWITCH_BRANCH(VK_TIMEOUT);
        SWITCH_BRANCH(VK_EVENT_SET);
        SWITCH_BRANCH(VK_EVENT_RESET);
        SWITCH_BRANCH(VK_INCOMPLETE);
        SWITCH_BRANCH(VK_ERROR_OUT_OF_HOST_MEMORY);
        SWITCH_BRANCH(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        SWITCH_BRANCH(VK_ERROR_INITIALIZATION_FAILED);
        SWITCH_BRANCH(VK_ERROR_DEVICE_LOST);
        SWITCH_BRANCH(VK_ERROR_MEMORY_MAP_FAILED);
        SWITCH_BRANCH(VK_ERROR_LAYER_NOT_PRESENT);
        SWITCH_BRANCH(VK_ERROR_EXTENSION_NOT_PRESENT);
        SWITCH_BRANCH(VK_ERROR_FEATURE_NOT_PRESENT);
        SWITCH_BRANCH(VK_ERROR_INCOMPATIBLE_DRIVER);
        SWITCH_BRANCH(VK_ERROR_TOO_MANY_OBJECTS);
        SWITCH_BRANCH(VK_ERROR_FORMAT_NOT_SUPPORTED);
        SWITCH_BRANCH(VK_ERROR_FRAGMENTED_POOL);
        SWITCH_BRANCH(VK_ERROR_UNKNOWN);
        SWITCH_BRANCH(VK_ERROR_OUT_OF_POOL_MEMORY);
        SWITCH_BRANCH(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        SWITCH_BRANCH(VK_ERROR_FRAGMENTATION);
        SWITCH_BRANCH(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
        SWITCH_BRANCH(VK_PIPELINE_COMPILE_REQUIRED);
        SWITCH_BRANCH(VK_ERROR_SURFACE_LOST_KHR);
        SWITCH_BRANCH(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        SWITCH_BRANCH(VK_SUBOPTIMAL_KHR);
        SWITCH_BRANCH(VK_ERROR_OUT_OF_DATE_KHR);
        SWITCH_BRANCH(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        SWITCH_BRANCH(VK_ERROR_VALIDATION_FAILED_EXT);
        SWITCH_BRANCH(VK_ERROR_INVALID_SHADER_NV);
        SWITCH_BRANCH(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
        SWITCH_BRANCH(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
        SWITCH_BRANCH(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
        SWITCH_BRANCH(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
        SWITCH_BRANCH(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
        SWITCH_BRANCH(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
        SWITCH_BRANCH(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        SWITCH_BRANCH(VK_ERROR_NOT_PERMITTED_KHR);
        SWITCH_BRANCH(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        SWITCH_BRANCH(VK_THREAD_IDLE_KHR);
        SWITCH_BRANCH(VK_THREAD_DONE_KHR);
        SWITCH_BRANCH(VK_OPERATION_DEFERRED_KHR);
        SWITCH_BRANCH(VK_OPERATION_NOT_DEFERRED_KHR);
        SWITCH_BRANCH(VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR);
        SWITCH_BRANCH(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
        SWITCH_BRANCH(VK_INCOMPATIBLE_SHADER_BINARY_EXT);
        SWITCH_BRANCH(VK_RESULT_MAX_ENUM);
#undef SWITCH_BRANCH
    default:
        return nullptr;
    }
}



GPUFormat transfer_format_reverse(VkFormat format)
{
	switch (format)
	{
	case VK_FORMAT_UNDEFINED: return GPUFormat::e_undefined;
	case VK_FORMAT_R4G4_UNORM_PACK8: return GPUFormat::e_r4g4_unorm;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return GPUFormat::e_r4g4b4a4_unorm;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return GPUFormat::e_b4g4r4a4_unorm;
	case VK_FORMAT_R5G6B5_UNORM_PACK16: return GPUFormat::e_r5g6b5_unorm;
	case VK_FORMAT_B5G6R5_UNORM_PACK16: return GPUFormat::e_b5g6r5_unorm;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return GPUFormat::e_r5g5b5a1_unorm;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return GPUFormat::e_b5g5r5a1_unorm;
	case VK_FORMAT_R8_UNORM: return GPUFormat::e_r8_unorm;
	case VK_FORMAT_R8_SNORM: return GPUFormat::e_r8_snorm;
	case VK_FORMAT_R8_UINT: return GPUFormat::e_r8_uint;
	case VK_FORMAT_R8_SINT: return GPUFormat::e_r8_sint;
	case VK_FORMAT_R8_SRGB: return GPUFormat::e_r8_srgb;
	case VK_FORMAT_R8G8_UNORM: return GPUFormat::e_r8g8_unorm;
	case VK_FORMAT_R8G8_SNORM: return GPUFormat::e_r8g8_snorm;
	case VK_FORMAT_R8G8_UINT: return GPUFormat::e_r8g8_uint;
	case VK_FORMAT_R8G8_SINT: return GPUFormat::e_r8g8_sint;
	case VK_FORMAT_R8G8_SRGB: return GPUFormat::e_r8g8_srgb;
	case VK_FORMAT_R8G8B8_UNORM: return GPUFormat::e_r8g8b8_unorm;
	case VK_FORMAT_R8G8B8_SNORM: return GPUFormat::e_r8g8b8_snorm;
	case VK_FORMAT_R8G8B8_UINT: return GPUFormat::e_r8g8b8_uint;
	case VK_FORMAT_R8G8B8_SINT: return GPUFormat::e_r8g8b8_sint;
	case VK_FORMAT_R8G8B8_SRGB: return GPUFormat::e_r8g8b8_srgb;
	case VK_FORMAT_B8G8R8_UNORM: return GPUFormat::e_b8g8r8_unorm;
	case VK_FORMAT_B8G8R8_SNORM: return GPUFormat::e_b8g8r8_snorm;
	case VK_FORMAT_B8G8R8_UINT: return GPUFormat::e_b8g8r8_uint;
	case VK_FORMAT_B8G8R8_SINT: return GPUFormat::e_b8g8r8_sint;
	case VK_FORMAT_B8G8R8_SRGB: return GPUFormat::e_b8g8r8_srgb;
	case VK_FORMAT_R8G8B8A8_UNORM: return GPUFormat::e_r8g8b8a8_unorm;
	case VK_FORMAT_R8G8B8A8_SNORM: return GPUFormat::e_r8g8b8a8_snorm;
	case VK_FORMAT_R8G8B8A8_UINT: return GPUFormat::e_r8g8b8a8_uint;
	case VK_FORMAT_R8G8B8A8_SINT: return GPUFormat::e_r8g8b8a8_sint;
	case VK_FORMAT_R8G8B8A8_SRGB: return GPUFormat::e_r8g8b8a8_srgb;
	case VK_FORMAT_B8G8R8A8_UNORM: return GPUFormat::e_b8g8r8a8_unorm;
	case VK_FORMAT_B8G8R8A8_SNORM: return GPUFormat::e_b8g8r8a8_snorm;
	case VK_FORMAT_B8G8R8A8_UINT: return GPUFormat::e_b8g8r8a8_uint;
	case VK_FORMAT_B8G8R8A8_SINT: return GPUFormat::e_b8g8r8a8_sint;
	case VK_FORMAT_B8G8R8A8_SRGB: return GPUFormat::e_b8g8r8_srgb;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return GPUFormat::e_r10g10b10a2_unorm;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
	case VK_FORMAT_A2B10G10R10_UINT_PACK32: return GPUFormat::e_r10g10b10a2_uint;
	case VK_FORMAT_R16_UNORM: return GPUFormat::e_r16_unorm;
	case VK_FORMAT_R16_SNORM: return GPUFormat::e_r16_snorm;
	case VK_FORMAT_R16_UINT: return GPUFormat::e_r16_uint;
	case VK_FORMAT_R16_SINT: return GPUFormat::e_r16_sint;
	case VK_FORMAT_R16_SFLOAT: return GPUFormat::e_r16_sfloat;
	case VK_FORMAT_R16G16_UNORM: return GPUFormat::e_r16g16_unorm;
	case VK_FORMAT_R16G16_SNORM: return GPUFormat::e_r16g16_snorm;
	case VK_FORMAT_R16G16_UINT: return GPUFormat::e_r16g16_uint;
	case VK_FORMAT_R16G16_SINT: return GPUFormat::e_r16g16_sint;
	case VK_FORMAT_R16G16_SFLOAT: return GPUFormat::e_r16g16_sfloat;
	case VK_FORMAT_R16G16B16_UNORM: return GPUFormat::e_r16g16b16_unorm;
	case VK_FORMAT_R16G16B16_SNORM: return GPUFormat::e_r16g16b16_snorm;
	case VK_FORMAT_R16G16B16_UINT: return GPUFormat::e_r16g16b16_uint;
	case VK_FORMAT_R16G16B16_SINT: return GPUFormat::e_r16g16b16_sint;
	case VK_FORMAT_R16G16B16_SFLOAT: return GPUFormat::e_r16g16b16_sfloat;
	case VK_FORMAT_R16G16B16A16_UNORM: return GPUFormat::e_r16g16b16a16_unorm;
	case VK_FORMAT_R16G16B16A16_SNORM: return GPUFormat::e_r16g16b16a16_snorm;
	case VK_FORMAT_R16G16B16A16_UINT: return GPUFormat::e_r16g16b16a16_uint;
	case VK_FORMAT_R16G16B16A16_SINT: return GPUFormat::e_r16g16b16a16_sint;
	case VK_FORMAT_R16G16B16A16_SFLOAT: return GPUFormat::e_r16g16b16a16_sfloat;
	case VK_FORMAT_R32_UINT: return GPUFormat::e_r32_uint;
	case VK_FORMAT_R32_SINT: return GPUFormat::e_r32_sint;
	case VK_FORMAT_R32_SFLOAT: return GPUFormat::e_r32_sfloat;
	case VK_FORMAT_R32G32_UINT: return GPUFormat::e_r32g32_uint;
	case VK_FORMAT_R32G32_SINT: return GPUFormat::e_r32g32_sint;
	case VK_FORMAT_R32G32_SFLOAT: return GPUFormat::e_r32g32_sfloat;
	case VK_FORMAT_R32G32B32_UINT: return GPUFormat::e_r32g32b32_uint;
	case VK_FORMAT_R32G32B32_SINT: return GPUFormat::e_r32g32b32_sint;
	case VK_FORMAT_R32G32B32_SFLOAT: return GPUFormat::e_r32g32b32_sfloat;
	case VK_FORMAT_R32G32B32A32_UINT: return GPUFormat::e_r32g32b32a32_uint;
	case VK_FORMAT_R32G32B32A32_SINT: return GPUFormat::e_r32g32b32a32_sint;
	case VK_FORMAT_R32G32B32A32_SFLOAT: return GPUFormat::e_r32g32b32a32_sfloat;
	case VK_FORMAT_R64_UINT: return GPUFormat::e_r64_uint;
	case VK_FORMAT_R64_SINT: return GPUFormat::e_r64_sint;
	case VK_FORMAT_R64_SFLOAT: return GPUFormat::e_r64_sfloat;
	case VK_FORMAT_R64G64_UINT: return GPUFormat::e_r64g64_uint;
	case VK_FORMAT_R64G64_SINT: return GPUFormat::e_r64g64_sint;
	case VK_FORMAT_R64G64_SFLOAT: return GPUFormat::e_r64g64_sfloat;
	case VK_FORMAT_R64G64B64_UINT: return GPUFormat::e_r64g64b64_uint;
	case VK_FORMAT_R64G64B64_SINT: return GPUFormat::e_r64g64b64_sint;
	case VK_FORMAT_R64G64B64_SFLOAT: return GPUFormat::e_r64g64b64_sfloat;
	case VK_FORMAT_R64G64B64A64_UINT: return GPUFormat::e_r64g64b64a64_uint;
	case VK_FORMAT_R64G64B64A64_SINT: return GPUFormat::e_r64g64b64a64_sint;
	case VK_FORMAT_R64G64B64A64_SFLOAT: return GPUFormat::e_r64g64b64a64_sfloat;

	case VK_FORMAT_D16_UNORM: return GPUFormat::e_d16_unorm;
	case VK_FORMAT_X8_D24_UNORM_PACK32: return GPUFormat::e_d24_x8_unorm;
	case VK_FORMAT_D32_SFLOAT: return GPUFormat::e_d32_sfloat;
	case VK_FORMAT_S8_UINT: return GPUFormat::e_s8_uint;
	case VK_FORMAT_D16_UNORM_S8_UINT: return GPUFormat::e_d16_unorm_s8_uint;
	case VK_FORMAT_D24_UNORM_S8_UINT: return GPUFormat::e_d24_unorm_s8_uint;
	case VK_FORMAT_D32_SFLOAT_S8_UINT: return GPUFormat::e_d32_sfloat_s8_uint;

	case VK_FORMAT_R8_USCALED:
	case VK_FORMAT_R8_SSCALED:
	case VK_FORMAT_R8G8_USCALED:
	case VK_FORMAT_R8G8_SSCALED:
	case VK_FORMAT_R8G8B8_USCALED:
	case VK_FORMAT_R8G8B8_SSCALED:
	case VK_FORMAT_B8G8R8_USCALED:
	case VK_FORMAT_B8G8R8_SSCALED:
	case VK_FORMAT_R8G8B8A8_USCALED:
	case VK_FORMAT_R8G8B8A8_SSCALED:
	case VK_FORMAT_B8G8R8A8_USCALED:
	case VK_FORMAT_B8G8R8A8_SSCALED:
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
	case VK_FORMAT_R16_USCALED:
	case VK_FORMAT_R16_SSCALED:
	case VK_FORMAT_R16G16_USCALED:
	case VK_FORMAT_R16G16_SSCALED:
	case VK_FORMAT_R16G16B16_USCALED:
	case VK_FORMAT_R16G16B16_SSCALED:
	case VK_FORMAT_R16G16B16A16_USCALED:
	case VK_FORMAT_R16G16B16A16_SSCALED:
	case VK_FORMAT_R10X6_UNORM_PACK16:
	case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
	case VK_FORMAT_R12X4_UNORM_PACK16:
	case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:

	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
	case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
	case VK_FORMAT_BC2_UNORM_BLOCK:
	case VK_FORMAT_BC2_SRGB_BLOCK:
	case VK_FORMAT_BC3_UNORM_BLOCK:
	case VK_FORMAT_BC3_SRGB_BLOCK:
	case VK_FORMAT_BC4_UNORM_BLOCK:
	case VK_FORMAT_BC4_SNORM_BLOCK:
	case VK_FORMAT_BC5_UNORM_BLOCK:
	case VK_FORMAT_BC5_SNORM_BLOCK:
	case VK_FORMAT_BC6H_UFLOAT_BLOCK:
	case VK_FORMAT_BC6H_SFLOAT_BLOCK:
	case VK_FORMAT_BC7_UNORM_BLOCK:
	case VK_FORMAT_BC7_SRGB_BLOCK:
	case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
	case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
	case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
	case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
	case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
	case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
	case VK_FORMAT_EAC_R11_UNORM_BLOCK:
	case VK_FORMAT_EAC_R11_SNORM_BLOCK:
	case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
	case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
	case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
	case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
	case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
	case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
	case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
	case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
	case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
	case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
	case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
	case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
	case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
	case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
	case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
	case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
	case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
	case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
	case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
	case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
	case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
	case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
	case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
	case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
	case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
	case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
	case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
	case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
	case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
	case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:

	case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
	case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
	case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
	case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:

	case VK_FORMAT_G16B16G16R16_422_UNORM:
	case VK_FORMAT_B16G16R16G16_422_UNORM:
	case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
	case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
	case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
	case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
	case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
	case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
	case VK_FORMAT_G8B8G8R8_422_UNORM:
	case VK_FORMAT_B8G8R8G8_422_UNORM:
	case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
	case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
	case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
	case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
	case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
	case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
	case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
	case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
	case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
	case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
	case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
	case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
	case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
	case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:

	case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
	case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
	case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
	case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
	default: return GPUFormat::e_undefined;
	}
}

VkFormat transfer_format(GPUFormat format)
{
    switch (format)
	{
    case GPUFormat::e_undefined: return VK_FORMAT_UNDEFINED;
	case GPUFormat::e_r4g4_unorm: return VK_FORMAT_R4G4_UNORM_PACK8;
	case GPUFormat::e_r4g4b4a4_unorm: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
	case GPUFormat::e_b4g4r4a4_unorm: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case GPUFormat::e_b5g6r5_unorm: return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case GPUFormat::e_r5g6b5_unorm: return VK_FORMAT_R5G6B5_UNORM_PACK16;
	case GPUFormat::e_r5g5b5a1_unorm: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
	case GPUFormat::e_b5g5r5a1_unorm: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case GPUFormat::e_r10g10b10a2_unorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case GPUFormat::e_r10g10b10a2_uint: return VK_FORMAT_A2B10G10R10_UINT_PACK32;

	case GPUFormat::e_r8_unorm: return VK_FORMAT_R8_UNORM;
	case GPUFormat::e_r8_snorm: return VK_FORMAT_R8_SNORM;
	case GPUFormat::e_r8_uint: return VK_FORMAT_R8_UINT;
	case GPUFormat::e_r8_sint: return VK_FORMAT_R8_SINT;
	case GPUFormat::e_r8_srgb: return VK_FORMAT_R8_SRGB;
	case GPUFormat::e_r8g8_unorm: return VK_FORMAT_R8G8_UNORM;
	case GPUFormat::e_r8g8_snorm: return VK_FORMAT_R8G8_SNORM;
	case GPUFormat::e_r8g8_uint: return VK_FORMAT_R8G8_UINT;
	case GPUFormat::e_r8g8_sint: return VK_FORMAT_R8G8_SINT;
	case GPUFormat::e_r8g8_srgb: return VK_FORMAT_R8G8_SRGB;
	case GPUFormat::e_r8g8b8_unorm: return VK_FORMAT_R8G8B8_UNORM;
	case GPUFormat::e_r8g8b8_snorm: return VK_FORMAT_R8G8B8_SNORM;
	case GPUFormat::e_r8g8b8_uint: return VK_FORMAT_R8G8B8_UINT;
	case GPUFormat::e_r8g8b8_sint: return VK_FORMAT_R8G8B8_SINT;
	case GPUFormat::e_r8g8b8_srgb: return VK_FORMAT_R8G8B8_SRGB;
	case GPUFormat::e_b8g8r8_unorm: return VK_FORMAT_B8G8R8_UNORM;
	case GPUFormat::e_b8g8r8_snorm: return VK_FORMAT_B8G8R8_SNORM;
	case GPUFormat::e_b8g8r8_uint: return VK_FORMAT_B8G8R8_UINT;
	case GPUFormat::e_b8g8r8_sint: return VK_FORMAT_B8G8R8_SINT;
	case GPUFormat::e_b8g8r8_srgb: return VK_FORMAT_B8G8R8_SRGB;
	case GPUFormat::e_r8g8b8a8_unorm: return VK_FORMAT_R8G8B8A8_UNORM;
	case GPUFormat::e_r8g8b8a8_snorm: return VK_FORMAT_R8G8B8A8_SNORM;
	case GPUFormat::e_r8g8b8a8_uint: return VK_FORMAT_R8G8B8A8_UINT;
	case GPUFormat::e_r8g8b8a8_sint: return VK_FORMAT_R8G8B8A8_SINT;
	case GPUFormat::e_r8g8b8a8_srgb: return VK_FORMAT_R8G8B8A8_SRGB;
	case GPUFormat::e_b8g8r8a8_unorm: return VK_FORMAT_B8G8R8A8_UNORM;
	case GPUFormat::e_b8g8r8a8_snorm: return VK_FORMAT_B8G8R8A8_SNORM;
	case GPUFormat::e_b8g8r8a8_uint: return VK_FORMAT_B8G8R8A8_UINT;
	case GPUFormat::e_b8g8r8a8_sint: return VK_FORMAT_B8G8R8A8_SINT;
	case GPUFormat::e_b8g8r8a8_srgb: return VK_FORMAT_B8G8R8A8_SRGB;
	case GPUFormat::e_r16_unorm: return VK_FORMAT_R16_UNORM;
	case GPUFormat::e_r16_snorm: return VK_FORMAT_R16_SNORM;
	case GPUFormat::e_r16_uint: return VK_FORMAT_R16_UINT;
	case GPUFormat::e_r16_sint: return VK_FORMAT_R16_SINT;
	case GPUFormat::e_r16_sfloat: return VK_FORMAT_R16_SFLOAT;
	case GPUFormat::e_r16g16_unorm: return VK_FORMAT_R16G16_UNORM;
	case GPUFormat::e_r16g16_snorm: return VK_FORMAT_R16G16_SNORM;
	case GPUFormat::e_r16g16_uint: return VK_FORMAT_R16G16_UINT;
	case GPUFormat::e_r16g16_sint: return VK_FORMAT_R16G16_SINT;
	case GPUFormat::e_r16g16_sfloat: return VK_FORMAT_R16G16_SFLOAT;
	case GPUFormat::e_r16g16b16_unorm: return VK_FORMAT_R16G16B16_UNORM;
	case GPUFormat::e_r16g16b16_snorm: return VK_FORMAT_R16G16B16_SNORM;
	case GPUFormat::e_r16g16b16_uint: return VK_FORMAT_R16G16B16_UINT;
	case GPUFormat::e_r16g16b16_sint: return VK_FORMAT_R16G16B16_SINT;
	case GPUFormat::e_r16g16b16_sfloat: return VK_FORMAT_R16G16B16_SFLOAT;
	case GPUFormat::e_r16g16b16a16_unorm: return VK_FORMAT_R16G16B16A16_UNORM;
	case GPUFormat::e_r16g16b16a16_snorm: return VK_FORMAT_R16G16B16A16_SNORM;
	case GPUFormat::e_r16g16b16a16_uint: return VK_FORMAT_R16G16B16A16_UINT;
	case GPUFormat::e_r16g16b16a16_sint: return VK_FORMAT_R16G16B16A16_SINT;
	case GPUFormat::e_r16g16b16a16_sfloat: return VK_FORMAT_R16G16B16A16_SFLOAT;
	case GPUFormat::e_r32_uint: return VK_FORMAT_R32_UINT;
	case GPUFormat::e_r32_sint: return VK_FORMAT_R32_SINT;
	case GPUFormat::e_r32_sfloat: return VK_FORMAT_R32_SFLOAT;
	case GPUFormat::e_r32g32_uint: return VK_FORMAT_R32G32_UINT;
	case GPUFormat::e_r32g32_sint: return VK_FORMAT_R32G32_SINT;
	case GPUFormat::e_r32g32_sfloat: return VK_FORMAT_R32G32_SFLOAT;
	case GPUFormat::e_r32g32b32_uint: return VK_FORMAT_R32G32B32_UINT;
	case GPUFormat::e_r32g32b32_sint: return VK_FORMAT_R32G32B32_SINT;
	case GPUFormat::e_r32g32b32_sfloat: return VK_FORMAT_R32G32B32_SFLOAT;
	case GPUFormat::e_r32g32b32a32_uint: return VK_FORMAT_R32G32B32A32_UINT;
	case GPUFormat::e_r32g32b32a32_sint: return VK_FORMAT_R32G32B32A32_SINT;
	case GPUFormat::e_r32g32b32a32_sfloat: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case GPUFormat::e_r64_uint: return VK_FORMAT_R64_UINT;
	case GPUFormat::e_r64_sint: return VK_FORMAT_R64_SINT;
	case GPUFormat::e_r64_sfloat: return VK_FORMAT_R64_SFLOAT;
	case GPUFormat::e_r64g64_uint: return VK_FORMAT_R64G64_UINT;
	case GPUFormat::e_r64g64_sint: return VK_FORMAT_R64G64_SINT;
	case GPUFormat::e_r64g64_sfloat: return VK_FORMAT_R64G64_SFLOAT;
	case GPUFormat::e_r64g64b64_uint: return VK_FORMAT_R64G64B64_UINT;
	case GPUFormat::e_r64g64b64_sint: return VK_FORMAT_R64G64B64_SINT;
	case GPUFormat::e_r64g64b64_sfloat: return VK_FORMAT_R64G64B64_SFLOAT;
	case GPUFormat::e_r64g64b64a64_uint: return VK_FORMAT_R64G64B64A64_UINT;
	case GPUFormat::e_r64g64b64a64_sint: return VK_FORMAT_R64G64B64A64_SINT;
	case GPUFormat::e_r64g64b64a64_sfloat: return VK_FORMAT_R64G64B64A64_SFLOAT;

	case GPUFormat::e_d16_unorm: return VK_FORMAT_D16_UNORM;
	case GPUFormat::e_d24_x8_unorm: return VK_FORMAT_X8_D24_UNORM_PACK32;
	case GPUFormat::e_d32_sfloat: return VK_FORMAT_D32_SFLOAT;
	case GPUFormat::e_s8_uint: return VK_FORMAT_S8_UINT;
	case GPUFormat::e_d16_unorm_s8_uint: return VK_FORMAT_D16_UNORM_S8_UINT;
	case GPUFormat::e_d24_unorm_s8_uint: return VK_FORMAT_D24_UNORM_S8_UINT;
	case GPUFormat::e_d32_sfloat_s8_uint: return VK_FORMAT_D32_SFLOAT_S8_UINT;

	default: return VK_FORMAT_UNDEFINED;
	}
}

VkBufferUsageFlags transfer_buffer_usage(GPUResourceType type, bool texture)
{
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	if (FLAG_IDENTITY(type, GPUResourceType::e_uniform_buffer))
		usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	if (FLAG_IDENTITY(type, GPUResourceType::e_rw_buffer))
	{
		usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (texture)
			usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
	}
	if (FLAG_IDENTITY(type, GPUResourceType::e_vertex_buffer))
		usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if (FLAG_IDENTITY(type, GPUResourceType::e_index_buffer))
		usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	if (FLAG_IDENTITY(type, GPUResourceType::e_indirect_buffer))
		usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	if (FLAG_IDENTITY(type, GPUResourceType::e_ray_tracing))
		usage |= VK_BUFFER_USAGE_RAY_TRACING_BIT_NV;
	return usage;
}

VkSampleCountFlagBits transfer_sample_count(GPUSampleCount sample_count)
{
	switch (sample_count)
	{
	case GPUSampleCount::e_2:
		return VK_SAMPLE_COUNT_2_BIT;
	case GPUSampleCount::e_4:
		return VK_SAMPLE_COUNT_4_BIT;
	case GPUSampleCount::e_8:
		return VK_SAMPLE_COUNT_8_BIT;
	case GPUSampleCount::e_16:
		return VK_SAMPLE_COUNT_16_BIT;
	case GPUSampleCount::e_32:
		return VK_SAMPLE_COUNT_32_BIT;
	case GPUSampleCount::e_64:
		return VK_SAMPLE_COUNT_64_BIT;
	case GPUSampleCount::e_1:
	default:
		return VK_SAMPLE_COUNT_1_BIT;
	}
}

VkImageUsageFlags transfer_image_usage(const GPUResourceType& type)
{
	VkImageUsageFlags usage = 0;
	if (FLAG_IDENTITY(type, GPUResourceType::e_texture))
		usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if (FLAG_IDENTITY(type, GPUResourceType::e_rw_buffer) || FLAG_IDENTITY(type, GPUResourceType::e_rw_texture))
		usage |= VK_IMAGE_USAGE_STORAGE_BIT;
	return usage;
}

VkImageAspectFlags transfer_image_aspect(VkFormat format, bool stencil)
{
	VkImageAspectFlags result = VK_IMAGE_ASPECT_NONE;
	switch (format)
	{
		// Depth
	case VK_FORMAT_D16_UNORM:
	case VK_FORMAT_X8_D24_UNORM_PACK32:
	case VK_FORMAT_D32_SFLOAT:
		result = VK_IMAGE_ASPECT_DEPTH_BIT;
		break;
		// Stencil
	case VK_FORMAT_S8_UINT:
		result = VK_IMAGE_ASPECT_STENCIL_BIT;
		break;
		// Depth/stencil
	case VK_FORMAT_D16_UNORM_S8_UINT:
	case VK_FORMAT_D24_UNORM_S8_UINT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		result = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (stencil)
			result |= VK_IMAGE_ASPECT_STENCIL_BIT;
		break;
		// Assume everything else is Color
	default:
		result = VK_IMAGE_ASPECT_COLOR_BIT;
		break;
	}
	return result;
}

VkFilter transfer_filter(GPUFilterType filter)
{
	switch (filter)
	{
	case GPUFilterType::e_linear: return VK_FILTER_LINEAR;
	case GPUFilterType::e_nearset: return VK_FILTER_NEAREST;
	default: return VK_FILTER_LINEAR;
	}
}

VkSamplerMipmapMode transfer_mipmap_mode(GPUMipMapMode mode)
{
	switch (mode)
	{
	case GPUMipMapMode::e_linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	case GPUMipMapMode::e_nearset: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	default: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}
}

VkSamplerAddressMode transfer_address_mode(GPUAddressMode mode)
{
	switch (mode)
	{
	case GPUAddressMode::e_repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case GPUAddressMode::e_mirror: return 	VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case GPUAddressMode::e_clamp_to_edge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case GPUAddressMode::e_clamp_to_border: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}

VkShaderStageFlags transfer_shader_stage(GPUShaderStage stage)
{
	if (FLAG_IDENTITY(stage, GPUShaderStage::e_all_graphics))
		return VK_SHADER_STAGE_ALL_GRAPHICS;
	else if (FLAG_IDENTITY(stage, GPUShaderStage::e_vertex))
		return VK_SHADER_STAGE_VERTEX_BIT;
	else if (FLAG_IDENTITY(stage, GPUShaderStage::e_tessellation_control))
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	else if (FLAG_IDENTITY(stage, GPUShaderStage::e_tessellation_evaluation))
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	else if (FLAG_IDENTITY(stage, GPUShaderStage::e_geometry))
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	else if (FLAG_IDENTITY(stage, GPUShaderStage::e_fragment))
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	else if (FLAG_IDENTITY(stage, GPUShaderStage::e_compute))
		return VK_SHADER_STAGE_COMPUTE_BIT;
	else
		return VK_SHADER_STAGE_ALL;
}

VkDescriptorType transfer_resource_type(GPUResourceType type)
{
	switch (type)
	{
	case GPUResourceType::e_sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
	case GPUResourceType::e_buffer:
	case GPUResourceType::e_texture: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case GPUResourceType::e_uniform_buffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case GPUResourceType::e_rw_texture:
	case GPUResourceType::e_rw_buffer: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case GPUResourceType::e_input_attachment: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	case GPUResourceType::e_texel_buffer: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	case GPUResourceType::e_rw_texel_buffer: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	case GPUResourceType::e_combined_image_sampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case GPUResourceType::e_ray_tracing: return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
	default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

VkPrimitiveTopology transfer_primitive_topology(GPUPrimitiveTopology topology)
{
	switch (topology)
	{
	case GPUPrimitiveTopology::e_point_list: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case GPUPrimitiveTopology::e_line_list: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case GPUPrimitiveTopology::e_line_strip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case GPUPrimitiveTopology::e_triangle_list: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case GPUPrimitiveTopology::e_triangle_strip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case GPUPrimitiveTopology::e_patch_list: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	default: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
}

VkAccessFlags transfer_access_state(GPUResourceState state)
{
	VkAccessFlags ret = VK_ACCESS_NONE;
	if (FLAG_IDENTITY(state, GPUResourceState::e_copy_source))
		ret |= VK_ACCESS_TRANSFER_READ_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_copy_destination))
		ret |= VK_ACCESS_TRANSFER_WRITE_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_vertex_and_constant_buffer))
		ret |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_index_buffer))
		ret |= VK_ACCESS_INDEX_READ_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_unordered_access))
		ret |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_indirect_argument))
		ret |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_render_target))
		ret |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_resolve_destination))
		ret |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_depth_write))
		ret |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_shader_resource))
		ret |= VK_ACCESS_SHADER_READ_BIT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_present))
		ret |= VK_ACCESS_NONE;
	if (FLAG_IDENTITY(state, GPUResourceState::e_acceleration_structure))
		ret |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV;
	return ret;
}

VkImageLayout transfer_image_layout(GPUResourceState state)
{
	if (FLAG_IDENTITY(state, GPUResourceState::e_copy_source))
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	if (FLAG_IDENTITY(state, GPUResourceState::e_copy_destination))
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	if (FLAG_IDENTITY(state, GPUResourceState::e_unordered_access))
        return VK_IMAGE_LAYOUT_GENERAL;
	if (FLAG_IDENTITY(state, GPUResourceState::e_render_target))
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	if (FLAG_IDENTITY(state, GPUResourceState::e_resolve_destination))
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	if (FLAG_IDENTITY(state, GPUResourceState::e_depth_write))
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	if (state & GPUResourceState::e_non_pixel_shader_resource ||
		state & GPUResourceState::e_pixel_shader_resource)
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	if (FLAG_IDENTITY(state, GPUResourceState::e_present))
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	if (FLAG_IDENTITY(state, GPUResourceState::e_shading_rate_source))
		return VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT;
	if (FLAG_IDENTITY(state, GPUResourceState::e_common))
		return VK_IMAGE_LAYOUT_GENERAL;

	return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkPipelineStageFlags transfer_pipeline_stage(GPUAdapter const* adapter, VkAccessFlags access_flags, GPUQueueType queue_type)
{
	VkPipelineStageFlags flags = VK_PIPELINE_STAGE_NONE;
	switch (queue_type)
	{
	case GPUQueueType::e_graphics:
	{
		if (access_flags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT))
			flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		if (access_flags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT))
		{
			flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			if (adapter->m_adapter_detail.support_geom_shader)
				flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			if (adapter->m_adapter_detail.support_tessellation)
			{
				flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
				flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
			}
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		if (access_flags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT)
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		if (access_flags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT))
			flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if (access_flags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT))
			flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		break;
	}
	case GPUQueueType::e_compute:
	{
		if (access_flags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
			VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT))
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		if (access_flags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT))
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		break;
	}
	case GPUQueueType::e_transfer: return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	default: break;
	}

	if (access_flags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT)
		flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
	if (access_flags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT))
		flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	if (access_flags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT))
		flags |= VK_PIPELINE_STAGE_HOST_BIT;

	if (flags == VK_ACCESS_NONE)
		flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	return flags;
}

AMAZING_NAMESPACE_END