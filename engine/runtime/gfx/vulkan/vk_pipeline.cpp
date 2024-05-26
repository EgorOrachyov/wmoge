/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

#include "vk_pipeline.hpp"

#include "core/task.hpp"
#include "core/timer.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

namespace wmoge {

    VKPsoLayout::VKPsoLayout(const GfxDescSetLayouts& layouts, const Strid& name, VKDriver& driver) : VKResource<GfxPsoLayout>(driver) {
        m_name    = name;
        m_layouts = layouts;

        VkDescriptorSetLayout vk_layouts[GfxLimits::MAX_DESC_SETS];
        std::uint32_t         vk_layouts_count = 0;

        for (const auto& layout : layouts) {
            assert(vk_layouts_count < GfxLimits::MAX_DESC_SETS);
            vk_layouts[vk_layouts_count++] = layout.cast<VKDescSetLayout>()->layout();
        }

        VkPipelineLayoutCreateInfo layout_create_info{};
        layout_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layout_create_info.setLayoutCount         = vk_layouts_count;
        layout_create_info.pSetLayouts            = vk_layouts;
        layout_create_info.pushConstantRangeCount = 0;
        layout_create_info.pPushConstantRanges    = nullptr;
        WG_VK_CHECK(vkCreatePipelineLayout(m_driver.device(), &layout_create_info, nullptr, &m_layout));
        WG_VK_NAME(m_driver.device(), m_layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, name.str());
    }

    VKPsoLayout::~VKPsoLayout() {
        if (m_layout) {
            vkDestroyPipelineLayout(m_driver.device(), m_layout, nullptr);
        }
    }

    VKPsoGraphics::VKPsoGraphics(const Strid& name, VKDriver& driver) : VKResource<GfxPsoGraphics>(driver) {
        m_name = name;
    }
    VKPsoGraphics::~VKPsoGraphics() {
        release();
    }

    Status VKPsoGraphics::compile(const GfxPsoStateGraphics& state) {
        WG_AUTO_PROFILE_VULKAN("VKPsoGraphics::compile");

        Timer timer;
        timer.start();

        auto program = state.program.cast<VKShaderProgram>();
        auto layout  = state.layout.cast<VKPsoLayout>();

        std::array<VkPipelineShaderStageCreateInfo, 6> shader_stages{};
        std::uint32_t                                  shader_stages_cout = 0;

        for (const Ref<GfxShader>& shader : program->get_desc()) {
            const std::uint32_t idx   = shader_stages_cout++;
            shader_stages[idx].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stages[idx].stage  = VKDefs::get_shader_module_type(shader->get_module_type());
            shader_stages[idx].module = shader.cast<VKShader>()->module();
            shader_stages[idx].pName  = "main";
        }

        auto render_pass = state.pass.cast<VKRenderPass>();
        auto vert_format = state.vert_format.cast<VKVertFormat>();

        const GfxRasterState&       rs = state.rs;
        const GfxDepthStencilState& ds = state.ds;
        const GfxBlendState&        bs = state.bs;

        VkPipelineVertexInputStateCreateInfo vertex_input_state{};
        vertex_input_state.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state.vertexBindingDescriptionCount   = vert_format->buffers_count();
        vertex_input_state.pVertexBindingDescriptions      = vert_format->buffers().data();
        vertex_input_state.vertexAttributeDescriptionCount = vert_format->attributes_count();
        vertex_input_state.pVertexAttributeDescriptions    = vert_format->attributes().data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology               = VKDefs::get_prim_type(state.prim_type);
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount  = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode             = VKDefs::get_poly_mode(rs.poly_mode);
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = VKDefs::get_poly_cull_mode(rs.cull_mode);
        rasterizer.frontFace               = VKDefs::get_poly_front_face(rs.front_face);
        rasterizer.depthBiasEnable         = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp          = 0.0f;
        rasterizer.depthBiasSlopeFactor    = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable   = VK_FALSE;
        multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading      = 1.0f;
        multisampling.pSampleMask           = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable      = VK_FALSE;

        VkStencilOpState stencil{};
        stencil.reference   = ds.stencil_rvalue;
        stencil.compareMask = ds.stencil_cmask;
        stencil.writeMask   = ds.stencil_wmask;
        stencil.compareOp   = VKDefs::get_comp_func(ds.stencil_comp_func);
        stencil.failOp      = VKDefs::get_stencil_op(ds.stencil_sfail);
        stencil.depthFailOp = VKDefs::get_stencil_op(ds.stencil_dfail);
        stencil.passOp      = VKDefs::get_stencil_op(ds.stencil_dpass);

        VkPipelineDepthStencilStateCreateInfo depth_stencil{};
        depth_stencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable       = ds.depth_enable;
        depth_stencil.depthWriteEnable      = ds.depth_write;
        depth_stencil.depthCompareOp        = VKDefs::get_comp_func(ds.depth_func);
        depth_stencil.depthBoundsTestEnable = false;
        depth_stencil.minDepthBounds        = 0.0f;
        depth_stencil.maxDepthBounds        = 1.0f;
        depth_stencil.stencilTestEnable     = ds.stencil_enable;
        depth_stencil.front                 = stencil;
        depth_stencil.back                  = stencil;

        VkPipelineColorBlendAttachmentState blend_attachment{};
        blend_attachment.blendEnable    = false;
        blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        std::array<VkPipelineColorBlendAttachmentState, GfxLimits::MAX_COLOR_TARGETS> blend_attachments;
        blend_attachments.fill(blend_attachment);

        if (bs.blending) {
            blend_attachments[0].blendEnable         = true;
            blend_attachments[0].alphaBlendOp        = VK_BLEND_OP_ADD;
            blend_attachments[0].colorBlendOp        = VK_BLEND_OP_ADD;
            blend_attachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            blend_attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            blend_attachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            blend_attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        }

        VkPipelineColorBlendStateCreateInfo color_blending{};
        color_blending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable     = VK_FALSE;
        color_blending.logicOp           = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount   = render_pass->color_targets_count();
        color_blending.pAttachments      = blend_attachments.data();
        color_blending.blendConstants[0] = 0.0f;
        color_blending.blendConstants[1] = 0.0f;
        color_blending.blendConstants[2] = 0.0f;
        color_blending.blendConstants[3] = 0.0f;

        std::array<VkDynamicState, 2> dynamic_states = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates    = dynamic_states.data();

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount          = shader_stages_cout;
        pipeline_info.pStages             = shader_stages.data();
        pipeline_info.pVertexInputState   = &vertex_input_state;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState      = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState   = &multisampling;
        pipeline_info.pDepthStencilState  = &depth_stencil;
        pipeline_info.pColorBlendState    = &color_blending;
        pipeline_info.pDynamicState       = &dynamic_state;
        pipeline_info.layout              = layout->layout();
        pipeline_info.renderPass          = render_pass->render_pass();
        pipeline_info.subpass             = 0;
        pipeline_info.basePipelineHandle  = m_pipeline;
        pipeline_info.basePipelineIndex   = -1;

        VkPipeline new_pipeline;

        auto result = vkCreateGraphicsPipelines(m_driver.device(), m_driver.pipeline_cache(), 1, &pipeline_info, nullptr, &new_pipeline);
        if (result != VK_SUCCESS) {
            WG_LOG_ERROR("failed to compile: " << name());
            return StatusCode::Error;
        }

        WG_VK_NAME(m_driver.device(), new_pipeline, VK_OBJECT_TYPE_PIPELINE, name().str());

        release();
        m_pipeline = new_pipeline;
        m_layout   = layout;

        timer.stop();
        WG_LOG_INFO("compiled: " << name() << " time: " << timer.get_elapsed_sec() << " sec");

        return WG_OK;
    }
    void VKPsoGraphics::release() {
        WG_AUTO_PROFILE_VULKAN("VKPsoGraphics::release");

        if (m_pipeline) {
            vkDestroyPipeline(m_driver.device(), m_pipeline, nullptr);
            m_pipeline = VK_NULL_HANDLE;
        }
    }

    VKPsoCompute::VKPsoCompute(const Strid& name, VKDriver& driver) : VKResource<GfxPsoCompute>(driver) {
        m_name = name;
    }
    VKPsoCompute::~VKPsoCompute() {
        release();
    }
    Status VKPsoCompute::compile(const GfxPsoStateCompute& state) {
        WG_AUTO_PROFILE_VULKAN("VKPsoCompute::compile");

        Timer timer;
        timer.start();

        auto program = state.program.cast<VKShaderProgram>();
        auto layout  = state.layout.cast<VKPsoLayout>();

        VkPipelineShaderStageCreateInfo shader_stage{};
        shader_stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
        shader_stage.module = program->get_desc()[0].cast<VKShader>()->module();
        shader_stage.pName  = "main";

        VkComputePipelineCreateInfo pipeline_info{};
        pipeline_info.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipeline_info.layout = layout->layout();
        pipeline_info.stage  = shader_stage;

        VkPipeline new_pipeline;

        auto result = vkCreateComputePipelines(m_driver.device(), m_driver.pipeline_cache(), 1, &pipeline_info, nullptr, &new_pipeline);
        if (result != VK_SUCCESS) {
            WG_LOG_ERROR("failed to compile: " << name());
            return StatusCode::Error;
        }

        WG_VK_NAME(m_driver.device(), new_pipeline, VK_OBJECT_TYPE_PIPELINE, name().str());

        m_pipeline = new_pipeline;
        m_layout   = layout;

        timer.stop();
        WG_LOG_INFO("compiled: " << name() << " time: " << timer.get_elapsed_sec() << " sec");

        return WG_OK;
    }
    void VKPsoCompute::release() {
        WG_AUTO_PROFILE_VULKAN("VKPsoCompute::release");

        if (m_pipeline) {
            vkDestroyPipeline(m_driver.device(), m_pipeline, nullptr);
            m_pipeline = VK_NULL_HANDLE;
        }
    }

}// namespace wmoge