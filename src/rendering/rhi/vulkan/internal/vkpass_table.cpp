//
// Created by AmazingBuff on 2025/5/13.
//

#include "vkpass_table.h"

AMAZING_NAMESPACE_BEGIN

VKPassTable::~VKPassTable()
{
    for (auto& [key, render_pass] : m_render_pass_map)
    {
        PLACEMENT_DELETE(VKRenderPass, render_pass);
    }
    for (auto& [key, framebuffer] : m_framebuffer_map)
    {
        PLACEMENT_DELETE(VKFramebuffer, framebuffer);
    }
}

VkRenderPass VKPassTable::find_render_pass(VKDevice const* device, VulkanRenderPassCreateInfo const& info)
{
    if (auto iter = m_render_pass_map.find(info); iter != m_render_pass_map.end())
        return iter->second->m_render_pass;

    VKRenderPass* render_pass = PLACEMENT_NEW(VKRenderPass, sizeof(VKRenderPass), device, info);
    m_render_pass_map.emplace(info, render_pass);
    return m_render_pass_map.find(info)->second->m_render_pass;
}

VkFramebuffer VKPassTable::find_framebuffer(VKDevice const* device, VulkanFramebufferCreateInfo const& info)
{
    if (auto iter = m_framebuffer_map.find(info); iter != m_framebuffer_map.end())
        return iter->second->m_framebuffer;

    VKFramebuffer* framebuffer = PLACEMENT_NEW(VKFramebuffer, sizeof(VKFramebuffer), device, info);
    m_framebuffer_map.emplace(info, framebuffer);
    return m_framebuffer_map.find(info)->second->m_framebuffer;
}

AMAZING_NAMESPACE_END