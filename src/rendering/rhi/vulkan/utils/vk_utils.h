//
// Created by AmazingBuff on 2025/5/6.
//

#ifndef VK_UTILS_H
#define VK_UTILS_H

#include "vk_macro.h"
#include "rendering/rhi/enumeration.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

static VkCompareOp Compare_Mode_Map[] =
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
