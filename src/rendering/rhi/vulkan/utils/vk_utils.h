//
// Created by AmazingBuff on 2025/5/6.
//

#ifndef VK_UTILS_H
#define VK_UTILS_H

#include "rendering/rhi/enumeration.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

static constexpr VkQueryType Query_Type_Map[] =
{
	VK_QUERY_TYPE_OCCLUSION,
	VK_QUERY_TYPE_TIMESTAMP,
	VK_QUERY_TYPE_PIPELINE_STATISTICS,
};

static constexpr VkCompareOp Compare_Mode_Map[] =
{
	VK_COMPARE_OP_NEVER,
	VK_COMPARE_OP_LESS,
	VK_COMPARE_OP_EQUAL,
	VK_COMPARE_OP_LESS_OR_EQUAL,
	VK_COMPARE_OP_GREATER,
	VK_COMPARE_OP_NOT_EQUAL,
	VK_COMPARE_OP_GREATER_OR_EQUAL,
	VK_COMPARE_OP_ALWAYS,
};

static constexpr VkPipelineBindPoint Pipeline_Bind_Point_Map[] =
{
	VK_PIPELINE_BIND_POINT_MAX_ENUM,
	VK_PIPELINE_BIND_POINT_COMPUTE,
	VK_PIPELINE_BIND_POINT_GRAPHICS,
	VK_PIPELINE_BIND_POINT_RAY_TRACING_NV
};

static constexpr VkStencilOp Stencil_Op_Map[] =
{
	VK_STENCIL_OP_KEEP,
	VK_STENCIL_OP_ZERO,
	VK_STENCIL_OP_REPLACE,
	VK_STENCIL_OP_INVERT,
	VK_STENCIL_OP_INCREMENT_AND_WRAP,
	VK_STENCIL_OP_DECREMENT_AND_WRAP,
	VK_STENCIL_OP_INCREMENT_AND_CLAMP,
	VK_STENCIL_OP_DECREMENT_AND_CLAMP,
};

static constexpr VkPolygonMode Polygon_Mode_Map[] =
{
	VK_POLYGON_MODE_FILL,
	VK_POLYGON_MODE_LINE
};

static constexpr VkCullModeFlagBits Cull_Mode_Map[] =
{
	VK_CULL_MODE_NONE,
	VK_CULL_MODE_BACK_BIT,
	VK_CULL_MODE_FRONT_BIT
};

static constexpr VkFrontFace Front_Face_Map[] =
{
	VK_FRONT_FACE_COUNTER_CLOCKWISE,
	VK_FRONT_FACE_CLOCKWISE
};

static constexpr VkBlendFactor Blend_Constant_Map[] =
{
	VK_BLEND_FACTOR_ZERO,
	VK_BLEND_FACTOR_ONE,
	VK_BLEND_FACTOR_SRC_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
	VK_BLEND_FACTOR_DST_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
	VK_BLEND_FACTOR_SRC_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	VK_BLEND_FACTOR_DST_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
	VK_BLEND_FACTOR_CONSTANT_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
};

static constexpr VkBlendOp Blend_Op_Map[] =
{
	VK_BLEND_OP_ADD,
	VK_BLEND_OP_SUBTRACT,
	VK_BLEND_OP_REVERSE_SUBTRACT,
	VK_BLEND_OP_MIN,
	VK_BLEND_OP_MAX,
};

static constexpr VkAttachmentLoadOp Attachment_Load_Op_Map[] =
{
	VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	VK_ATTACHMENT_LOAD_OP_LOAD,
	VK_ATTACHMENT_LOAD_OP_CLEAR,
};

static constexpr VkAttachmentStoreOp Attachment_Store_Op_Map[] =
{
	VK_ATTACHMENT_STORE_OP_DONT_CARE,
	VK_ATTACHMENT_STORE_OP_STORE,
	VK_ATTACHMENT_STORE_OP_DONT_CARE
};

const char* transfer_result(VkResult result);
VkFormat transfer_format(GPUFormat format);
GPUFormat transfer_format_reverse(VkFormat format);
VkBufferUsageFlags transfer_buffer_usage(GPUResourceType type, bool texture);
VkSampleCountFlagBits transfer_sample_count(GPUSampleCount sample_count);
VkImageUsageFlags transfer_image_usage(GPUResourceType type);
VkImageAspectFlags transfer_image_aspect(VkFormat format, bool stencil);
VkFilter transfer_filter(GPUFilterType filter);
VkSamplerMipmapMode transfer_mipmap_mode(GPUMipMapMode mode);
VkSamplerAddressMode transfer_address_mode(GPUAddressMode mode);
VkShaderStageFlags transfer_shader_stage(GPUShaderStage stage);
VkDescriptorType transfer_resource_type(GPUResourceType type);
VkPrimitiveTopology transfer_primitive_topology(GPUPrimitiveTopology topology);
VkAccessFlags transfer_access_state(GPUResourceState state);
VkImageLayout transfer_image_layout(GPUResourceState state);
VkPipelineStageFlags transfer_pipeline_stage(GPUAdapter const* adapter, VkAccessFlags access_flags, GPUQueueType queue_type);

template<typename F, typename... Args>
auto enumerate_properties(F&& func, Args&&... args)
{
	using type = std::remove_pointer_t<Trait::tail_type_t<typename Trait::function_traits<std::decay_t<F>>::argument_type>>;
	uint32_t count = 0;
	Vector<type> properties;
	if constexpr (!std::is_same_v<typename Trait::function_traits<std::decay_t<F>>::return_type, void>)
	{
		VK_CHECK_RESULT(Trait::invoke(std::forward<F>(func), std::forward<Args>(args)..., &count, nullptr));
		properties.resize(count);
		VK_CHECK_RESULT(Trait::invoke(std::forward<F>(func), std::forward<Args>(args)..., &count, properties.data()));
	}
	else
	{
		Trait::invoke(std::forward<F>(func), std::forward<Args>(args)..., &count, nullptr);
		properties.resize(count);
		Trait::invoke(std::forward<F>(func), std::forward<Args>(args)..., &count, properties.data());
	}
	return properties;
}

AMAZING_NAMESPACE_END

#endif //VK_UTILS_H
