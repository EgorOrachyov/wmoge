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
#include "vk_driver.hpp"

#include "core/engine.hpp"
#include "core/task.hpp"
#include "core/timer.hpp"
#include "debug/profiler.hpp"

namespace wmoge {

    VKPipeline::VKPipeline(const GfxPipelineState& state, const StringId& name, VKDriver& driver) : VKResource<GfxPipeline>(driver) {
        m_name  = name;
        m_state = state;
    }
    VKPipeline::~VKPipeline() {
        WG_AUTO_PROFILE_VULKAN("VKPipeline::~VKPipeline");

        release();
    }
    bool VKPipeline::validate(const Ref<VKRenderPass>& render_pass) {
        WG_AUTO_PROFILE_VULKAN("VKPipeline::validate");

        GfxPipelineStatus status = m_status.load();

        //  creating pipeline in a background task, wait
        if (status == GfxPipelineStatus::Creating) {
            return false;
        }
        // creating is failed, pipeline is broken, nothing to do
        if (status == GfxPipelineStatus::Failed) {
            return false;
        }
        // pipeline is not yet created or version out of date, have to create new pipeline
        if (status != GfxPipelineStatus::Created || m_render_pass != render_pass) {
            auto shader = m_state.shader.cast<VKShader>();
            if (shader->status() != GfxShaderStatus::Compiled) {
                return false;
            }

            assert(m_state.pass_desc == GfxRenderPassDesc{} || m_state.pass_desc == render_pass->pass_desc());

            m_render_pass = render_pass;
            m_status.store(GfxPipelineStatus::Creating);

            Task compilation_task(m_name, [self = Ref<VKPipeline>(this)](auto&) {
                self->compile();
                return 0;
            });

            compilation_task.schedule();

            return false;
        }

        // so everything is ok, can draw
        return true;
    }
    void VKPipeline::compile() {
        WG_AUTO_PROFILE_VULKAN("VKPipeline::compile");

        Timer timer;
        timer.start();

        auto shader = m_state.shader.cast<VKShader>();

        std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};
        shader_stages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].module = shader->modules()[0];
        shader_stages[0].pName  = "main";
        shader_stages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].module = shader->modules()[1];
        shader_stages[1].pName  = "main";

        auto vert_format = m_state.vert_format.cast<VKVertFormat>();

        VkPipelineVertexInputStateCreateInfo vertex_input_state{};
        vertex_input_state.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state.vertexBindingDescriptionCount   = vert_format->buffers_count();
        vertex_input_state.pVertexBindingDescriptions      = vert_format->buffers().data();
        vertex_input_state.vertexAttributeDescriptionCount = vert_format->attributes_count();
        vertex_input_state.pVertexAttributeDescriptions    = vert_format->attributes().data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology               = VKDefs::get_prim_type(m_state.prim_type);
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount  = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode             = VKDefs::get_poly_mode(m_state.poly_mode);
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = VKDefs::get_poly_cull_mode(m_state.cull_mode);
        rasterizer.frontFace               = VKDefs::get_poly_front_face(m_state.front_face);
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
        stencil.reference   = m_state.stencil_rvalue;
        stencil.compareMask = m_state.stencil_cmask;
        stencil.writeMask   = m_state.stencil_wmask;
        stencil.compareOp   = VKDefs::get_comp_func(m_state.stencil_comp_func);
        stencil.failOp      = VKDefs::get_stencil_op(m_state.stencil_sfail);
        stencil.depthFailOp = VKDefs::get_stencil_op(m_state.stencil_dfail);
        stencil.passOp      = VKDefs::get_stencil_op(m_state.stencil_dpass);

        VkPipelineDepthStencilStateCreateInfo depth_stencil{};
        depth_stencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable       = m_state.depth_enable;
        depth_stencil.depthWriteEnable      = m_state.depth_write;
        depth_stencil.depthCompareOp        = VKDefs::get_comp_func(m_state.depth_func);
        depth_stencil.depthBoundsTestEnable = false;
        depth_stencil.minDepthBounds        = 0.0f;
        depth_stencil.maxDepthBounds        = 1.0f;
        depth_stencil.stencilTestEnable     = m_state.stencil_enable;
        depth_stencil.front                 = stencil;
        depth_stencil.back                  = stencil;

        VkPipelineColorBlendAttachmentState blend_attachment{};
        blend_attachment.blendEnable    = false;
        blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        std::array<VkPipelineColorBlendAttachmentState, GfxLimits::MAX_COLOR_TARGETS> blend_attachments;
        blend_attachments.fill(blend_attachment);

        if (m_state.blending) {
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
        color_blending.attachmentCount   = m_render_pass->color_targets_count();
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
        pipeline_info.stageCount          = static_cast<uint32_t>(shader_stages.size());
        pipeline_info.pStages             = shader_stages.data();
        pipeline_info.pVertexInputState   = &vertex_input_state;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState      = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState   = &multisampling;
        pipeline_info.pDepthStencilState  = &depth_stencil;
        pipeline_info.pColorBlendState    = &color_blending;
        pipeline_info.pDynamicState       = &dynamic_state;
        pipeline_info.layout              = shader->layout();
        pipeline_info.renderPass          = m_render_pass->render_pass();
        pipeline_info.subpass             = 0;
        pipeline_info.basePipelineHandle  = m_pipeline;
        pipeline_info.basePipelineIndex   = -1;

        VkPipeline new_pipeline;

        WG_VK_CHECK(vkCreateGraphicsPipelines(m_driver.device(), m_driver.pipeline_cache(), 1, &pipeline_info, nullptr, &new_pipeline));
        WG_VK_NAME(m_driver.device(), new_pipeline, VK_OBJECT_TYPE_PIPELINE, "pipeline@" + name().str());

        release();
        m_pipeline = new_pipeline;

        timer.stop();
        WG_LOG_INFO("compiled: " << name() << " time: " << timer.get_elapsed_sec() << " sec");

        m_status.store(GfxPipelineStatus::Created);
    }
    void VKPipeline::release() {
        WG_AUTO_PROFILE_VULKAN("VKPipeline::release");

        if (m_pipeline) {
            m_driver.release_queue()->push([p = m_pipeline, d = m_driver.device()]() { vkDestroyPipeline(d, p, nullptr); });
            m_pipeline = VK_NULL_HANDLE;
        }
    }
    GfxPipelineStatus VKPipeline::status() const {
        return m_status.load();
    }
    std::string VKPipeline::message() const {
        return status() != GfxPipelineStatus::Creating ? m_message : std::string();
    }
    const GfxPipelineState& VKPipeline::state() const {
        return m_state;
    }

}// namespace wmoge