//
// Created by AmazingBuff on 2025/5/12.
//

#include "vkgraphics_pipeline.h"
#include "vkdevice.h"
#include "vkadapter.h"
#include "vkroot_signature.h"
#include "internal/vkpass_table.h"
#include "resources/vkshader_library.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKGraphicsPipeline::VKGraphicsPipeline(GPUGraphicsPipelineCreateInfo const& info) : m_pipeline(nullptr)
{
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(info.root_signature);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_root_signature->m_ref_device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    uint32_t input_binding_count = 0;
    uint32_t input_attribute_count = 0;
    VkVertexInputBindingDescription* input_binding_descriptions = nullptr;
    VkVertexInputAttributeDescription* input_attribute_descriptions = nullptr;
    if (!info.vertex_inputs.empty())
    {
        Map<uint32_t, VkVertexInputBindingDescription> input_bindings;
        for (GPUVertexAttribute const& vertex_input : info.vertex_inputs)
        {
            input_bindings.emplace(vertex_input.slot, VkVertexInputBindingDescription{});
            input_attribute_count += vertex_input.array_size;
        }
        input_binding_descriptions = STACK_NEW(VkVertexInputBindingDescription, input_bindings.size());
        input_attribute_descriptions = STACK_NEW(VkVertexInputAttributeDescription, input_attribute_count);

        uint32_t attribute_index = 0;
        for (GPUVertexAttribute const& vertex_input : info.vertex_inputs)
        {
            VkVertexInputBindingDescription& input_binding_description = input_bindings[vertex_input.slot];
            input_binding_description.binding = vertex_input.slot;
            input_binding_description.stride += vertex_input.size;
            if (vertex_input.rate == GPUVertexInputRate::e_instance)
                input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            else
                input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            for (uint32_t i = 0; i < vertex_input.array_size; ++i)
            {
                input_attribute_descriptions[attribute_index].binding = vertex_input.slot;
                input_attribute_descriptions[attribute_index].location = vertex_input.location;
                input_attribute_descriptions[attribute_index].format = transfer_format(vertex_input.format);
                input_attribute_descriptions[attribute_index].offset = vertex_input.offset;
                ++attribute_index;
            }
        }

        for (auto const& [binding, description] : input_bindings)
        {
            input_binding_descriptions[input_binding_count] = description;
            input_binding_count++;
        }
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = input_binding_count,
        .pVertexBindingDescriptions = input_binding_descriptions,
        .vertexAttributeDescriptionCount = input_attribute_count,
        .pVertexAttributeDescriptions = input_attribute_descriptions,
    };

    // shader stage
    const VkSpecializationInfo* specialization_info = nullptr;
    uint32_t stage_count = 0;
    VkPipelineShaderStageCreateInfo shader_stage_info[5]{};
#define SHADER_STAGE(name, vk_stage)                                                                                        \
    if (info.name)                                                                                                          \
    {                                                                                                                       \
        shader_stage_info[stage_count].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;                         \
        shader_stage_info[stage_count].pSpecializationInfo = specialization_info;                                           \
        shader_stage_info[stage_count].pName = info.name->entry.c_str();                                                    \
        shader_stage_info[stage_count].stage = vk_stage;                                                                    \
        shader_stage_info[stage_count].module = static_cast<VKShaderLibrary const*>(info.name->library)->m_shader_module;   \
        stage_count++;                                                                                                      \
    }

    SHADER_STAGE(vertex_shader, VK_SHADER_STAGE_VERTEX_BIT)
        SHADER_STAGE(tessellation_control_shader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
        SHADER_STAGE(tessellation_evaluation_shader, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
        SHADER_STAGE(geometry_shader, VK_SHADER_STAGE_GEOMETRY_BIT)
        SHADER_STAGE(fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT)
#undef SHADER_STAGE

        // viewport
        VkPipelineViewportStateCreateInfo viewport_state_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr,
    };

    // dynamic states
    Vector<VkDynamicState> dynamic_states(array_size(VK_Dynamic_States));
    memcpy(dynamic_states.data(), VK_Dynamic_States, array_size(VK_Dynamic_States) * sizeof(VkDynamicState));
    if (vk_adapter->m_adapter_detail.support_shading_rate)
        dynamic_states.emplace_back(VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR);

    VkPipelineDynamicStateCreateInfo dynamic_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates = dynamic_states.data(),
    };

    // multi sampling
    VkPipelineMultisampleStateCreateInfo multisample_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = transfer_sample_count(info.sample_count > GPUSampleCount::e_1 ? info.sample_count : GPUSampleCount::e_1),
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0.0,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    // input assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = transfer_primitive_topology(info.primitive_topology),
        .primitiveRestartEnable = VK_FALSE,
    };

    // depth stencil state
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
    };
    if (info.depth_stencil_state)
    {
        GPUDepthStencilState const* depth_stencil_state = info.depth_stencil_state;

        depth_stencil_state_info.depthTestEnable = depth_stencil_state->depth_test ? VK_TRUE : VK_FALSE;
        depth_stencil_state_info.depthWriteEnable = depth_stencil_state->depth_write ? VK_TRUE : VK_FALSE;
        depth_stencil_state_info.depthCompareOp = Compare_Mode_Map[to_underlying(depth_stencil_state->depth_compare)];
        depth_stencil_state_info.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_state_info.stencilTestEnable = depth_stencil_state->stencil_test ? VK_TRUE : VK_FALSE;

        depth_stencil_state_info.front.failOp = Stencil_Op_Map[to_underlying(depth_stencil_state->stencil_front_fail)];
        depth_stencil_state_info.front.passOp = Stencil_Op_Map[to_underlying(depth_stencil_state->stencil_front_pass)];
        depth_stencil_state_info.front.depthFailOp = Stencil_Op_Map[to_underlying(depth_stencil_state->depth_front_fail)];
        depth_stencil_state_info.front.compareOp = Compare_Mode_Map[to_underlying(depth_stencil_state->stencil_front_compare)];
        depth_stencil_state_info.front.compareMask = depth_stencil_state->stencil_read_mask;
        depth_stencil_state_info.front.writeMask = depth_stencil_state->stencil_write_mask;
        depth_stencil_state_info.front.reference = 0;

        depth_stencil_state_info.back.failOp = Stencil_Op_Map[to_underlying(depth_stencil_state->stencil_back_fail)];
        depth_stencil_state_info.back.passOp = Stencil_Op_Map[to_underlying(depth_stencil_state->stencil_back_pass)];
        depth_stencil_state_info.back.depthFailOp = Stencil_Op_Map[to_underlying(depth_stencil_state->depth_back_fail)];
        depth_stencil_state_info.back.compareOp = Compare_Mode_Map[to_underlying(depth_stencil_state->stencil_back_compare)];
        depth_stencil_state_info.back.compareMask = depth_stencil_state->stencil_read_mask;
        depth_stencil_state_info.back.writeMask = depth_stencil_state->stencil_write_mask;
        depth_stencil_state_info.back.reference = 0;
    }

    // rasterizer state
    VkPipelineRasterizationStateCreateInfo rasterization_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };
    if (info.rasterizer_state)
    {
        GPURasterizerState const* rasterizer_state = info.rasterizer_state;

        rasterization_state_info.depthBiasClamp = rasterizer_state->enable_depth_clamp ? VK_TRUE : VK_FALSE;
        rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state_info.polygonMode = Polygon_Mode_Map[to_underlying(rasterizer_state->fill_mode)];
        rasterization_state_info.cullMode = Cull_Mode_Map[to_underlying(rasterizer_state->cull_mode)];
        rasterization_state_info.frontFace = Front_Face_Map[to_underlying(rasterizer_state->front_face)];
        rasterization_state_info.depthBiasEnable = rasterizer_state->depth_bias != 0.0f ? VK_TRUE : VK_FALSE;
        rasterization_state_info.depthBiasConstantFactor = rasterizer_state->depth_bias;
        rasterization_state_info.depthBiasSlopeFactor = rasterizer_state->slope_scaled_depth_bias;
    }

    // blending state
    VkPipelineColorBlendAttachmentState color_blend_state[GPU_Max_Render_Target]{};

    if (info.blend_state)
    {
        GPUBlendState const* blend_state = info.blend_state;

        uint32_t blend_index = 0;
        for (uint32_t i = 0; i < info.render_target_count; i++)
        {
            VkPipelineColorBlendAttachmentState& rt_blend = color_blend_state[i];
            rt_blend.srcColorBlendFactor = Blend_Constant_Map[to_underlying(blend_state->src_factors[blend_index])];
            rt_blend.dstColorBlendFactor = Blend_Constant_Map[to_underlying(blend_state->dst_factors[blend_index])];
            rt_blend.srcAlphaBlendFactor = Blend_Constant_Map[to_underlying(blend_state->alpha_src_factors[blend_index])];
            rt_blend.dstAlphaBlendFactor = Blend_Constant_Map[to_underlying(blend_state->alpha_dst_factors[blend_index])];

            if (rt_blend.srcColorBlendFactor != VK_BLEND_FACTOR_ONE || rt_blend.dstColorBlendFactor != VK_BLEND_FACTOR_ZERO ||
                rt_blend.srcAlphaBlendFactor != VK_BLEND_FACTOR_ONE || rt_blend.dstAlphaBlendFactor != VK_BLEND_FACTOR_ZERO)
                rt_blend.blendEnable = VK_TRUE;
            else
                rt_blend.blendEnable = VK_FALSE;

            rt_blend.colorWriteMask = static_cast<VkColorComponentFlags>(blend_state->write_masks[blend_index]);
            rt_blend.colorBlendOp = Blend_Op_Map[to_underlying(blend_state->blend_modes[blend_index])];
            rt_blend.alphaBlendOp = Blend_Op_Map[to_underlying(blend_state->alpha_blend_modes[blend_index])];

            if (blend_state->independent_blend)
                blend_index++;
        }
    }
    else
    {
        for (uint32_t i = 0; i < info.render_target_count; i++)
        {
            VkPipelineColorBlendAttachmentState& rt_blend = color_blend_state[i];
            rt_blend.blendEnable = VK_FALSE;
            rt_blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                   VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        }
    }
    VkPipelineColorBlendStateCreateInfo blend_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = info.render_target_count,
        .pAttachments = color_blend_state,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
    };

    // render pass
    VulkanRenderPassCreateInfo render_pass_info{
        .color_attachment_count = info.render_target_count,
        .sample_count = info.sample_count > GPUSampleCount::e_1 ? info.sample_count : GPUSampleCount::e_1,
        .depth_stencil_attachment{
            .depth_stencil_format = info.depth_stencil_format,
        },
    };
    for (uint32_t i = 0; i < info.render_target_count; i++)
    {
        render_pass_info.color_attachment[i].format = info.color_format[i];
        if (info.sample_count > GPUSampleCount::e_1)
            render_pass_info.color_attachment[i].resolve_enable = true;
    }
    VkRenderPass render_pass = vk_device->m_pass_table->find_render_pass(vk_device, render_pass_info);

    VkGraphicsPipelineCreateInfo pipeline_info{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = stage_count,
        .pStages = shader_stage_info,
        .pVertexInputState = &vertex_input_state_info,
        .pInputAssemblyState = &input_assembly_state_info,
        .pViewportState = &viewport_state_info,
        .pRasterizationState = &rasterization_state_info,
        .pMultisampleState = &multisample_state_info,
        .pDepthStencilState = &depth_stencil_state_info,
        .pColorBlendState = &blend_state_info,
        .pDynamicState = &dynamic_state_info,
        .layout = vk_root_signature->m_pipeline_layout,
        .renderPass = render_pass,
        .subpass = 0,
        .basePipelineHandle = nullptr,
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateGraphicsPipelines(vk_device->m_device, vk_device->m_pipeline_cache, 1, &pipeline_info, VK_Allocation_Callbacks_Ptr, &m_pipeline));

    m_ref_root_signature = vk_root_signature;
}

VKGraphicsPipeline::~VKGraphicsPipeline()
{
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(m_ref_root_signature);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_root_signature->m_ref_device);

    vk_device->m_device_table.vkDestroyPipeline(vk_device->m_device, m_pipeline, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END