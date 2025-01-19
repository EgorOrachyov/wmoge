#include "rdg_pass.hpp"
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

#include "rdg_pass.hpp"

#include "gfx/gfx_driver.hpp"
#include "grc/shader_manager.hpp"

#include <cassert>

namespace wmoge {

    RdgPass::RdgPass(RdgGraph& graph, Strid name, RdgPassId id, RdgPassFlags flags) : m_graph(graph) {
        m_name  = name;
        m_id    = id;
        m_flags = flags;
    }

    RdgPass& RdgPass::color_target(RdgTexture* target) {
        RdgPassColorTarget& t = m_color_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::LoadStore;
        return reference(target, GfxAccess::RenderTarget);
    }

    RdgPass& RdgPass::color_target(RdgTexture* target, const Color4f& clear_color) {
        RdgPassColorTarget& t = m_color_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::ClearStore;
        t.color               = clear_color;
        t.clear               = true;
        return reference(target, GfxAccess::RenderTarget);
    }

    RdgPass& RdgPass::depth_target(RdgTexture* target) {
        assert(m_depth_targets.empty());
        RdgPassDepthTarget& t = m_depth_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::LoadStore;
        return reference(target, GfxAccess::RenderTarget);
    }

    RdgPass& RdgPass::depth_target(RdgTexture* target, float clear_depth, int clear_stencil) {
        RdgPassDepthTarget& t = m_depth_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::ClearStore;
        t.depth               = clear_depth;
        t.stencil             = clear_stencil;
        t.clear_depth         = true;
        t.clear_stencil       = true;
        return reference(target, GfxAccess::RenderTarget);
    }

    RdgPass& RdgPass::reference(RdgResource* resource, GfxAccess access) {
        assert(resource);
        assert(!has_resource(resource));

        RdgPassResource& r = m_resources.emplace_back();
        r.resource         = resource;
        r.access           = access;
        m_referenced.insert(resource);

        return *this;
    }

    RdgPass& RdgPass::uniform(RdgBuffer* resource) {
        return reference(resource, GfxAccess::Uniform);
    }

    RdgPass& RdgPass::reading(RdgBuffer* resource) {
        return reference(resource, GfxAccess::BufferRead);
    }

    RdgPass& RdgPass::writing(RdgBuffer* resource) {
        return reference(resource, GfxAccess::BufferWrite);
    }

    RdgPass& RdgPass::copy_source(RdgBuffer* resource) {
        return reference(resource, GfxAccess::CopySource);
    }

    RdgPass& RdgPass::copy_destination(RdgBuffer* resource) {
        return reference(resource, GfxAccess::CopyDestination);
    }

    RdgPass& RdgPass::sampling(RdgTexture* resource) {
        return reference(resource, GfxAccess::TexureSample);
    }

    RdgPass& RdgPass::storage(RdgTexture* resource) {
        return reference(resource, GfxAccess::ImageStore);
    }

    RdgPass& RdgPass::bind(RdgPassCallback callback) {
        m_callback = std::move(callback);
        return *this;
    }

    GfxRenderPassDesc RdgPass::make_render_pass_desc() const {
        GfxRenderPassDesc desc;

        for (int i = 0; i < m_color_targets.size(); i++) {
            desc.color_target_fmts[i] = m_color_targets[i].resource->get_desc().format;
            desc.color_target_ops[i]  = m_color_targets[i].op;
        }

        for (int i = 0; i < m_depth_targets.size(); i++) {
            desc.depth_stencil_fmt = m_depth_targets[i].resource->get_desc().format;
            desc.depth_op          = m_depth_targets[i].op;
            desc.stencil_op        = m_depth_targets[i].op;
        }

        return desc;
    }

    bool RdgPass::has_resource(RdgResource* r) const {
        return m_referenced.find(r) != m_referenced.end();
    }

    RdgPassContext::RdgPassContext(GfxCmdListRef cmd_list, GfxDriver* driver, ShaderManager* shader_manager, RdgGraph* graph)
        : m_cmd_list(std::move(cmd_list)), m_driver(driver), m_shader_manager(shader_manager), m_graph(graph) {
    }

    Status RdgPassContext::update_vert_buffer(GfxVertBuffer* buffer, int offset, int range, array_view<const std::uint8_t> data) {
        m_cmd_list->update_vert_buffer(buffer, offset, range, data);
        return WG_OK;
    }

    Status RdgPassContext::update_index_buffer(GfxIndexBuffer* buffer, int offset, int range, array_view<const std::uint8_t> data) {
        m_cmd_list->update_index_buffer(buffer, offset, range, data);
        return WG_OK;
    }

    Status RdgPassContext::update_uniform_buffer(GfxUniformBuffer* buffer, int offset, int range, array_view<const std::uint8_t> data) {
        m_cmd_list->update_uniform_buffer(buffer, offset, range, data);
        return WG_OK;
    }

    Status RdgPassContext::update_storage_buffer(GfxStorageBuffer* buffer, int offset, int range, array_view<const std::uint8_t> data) {
        m_cmd_list->update_storage_buffer(buffer, offset, range, data);
        return WG_OK;
    }

    Status RdgPassContext::validate_param_block(ShaderParamBlock* param_block) {
        m_shader_manager->validate_param_blocks({&param_block, 1}, m_cmd_list);
        return WG_OK;
    }

    Status RdgPassContext::bind_param_block(ShaderParamBlock* param_block) {
        WG_CHECKED(validate_param_block(param_block));
        m_cmd_list->bind_desc_set(param_block->get_gfx_set(), param_block->get_space());
        return WG_OK;
    }

    Status RdgPassContext::bind_pso_graphics(Shader* shader, const ShaderPermutation& permutation, const GfxVertElements& vert_elements) {
        GfxRenderPassRef render_pass;
        m_cmd_list->peek_render_pass(render_pass);
        GfxPsoGraphicsRef pso = m_shader_manager->get_or_create_pso_graphics(shader, permutation, render_pass, vert_elements);
        if (!pso) {
            return StatusCode::NoValue;
        }
        m_cmd_list->bind_pso(pso);
        return WG_OK;
    }

    Status RdgPassContext::bind_pso_compute(Shader* shader, const ShaderPermutation& permutation) {
        GfxPsoComputeRef pso = m_shader_manager->get_or_create_pso_compute(shader, permutation);
        if (!pso) {
            return StatusCode::NoValue;
        }
        m_cmd_list->bind_pso(pso);
        return WG_OK;
    }

    Status RdgPassContext::viewport(const Rect2i& viewport) {
        m_cmd_list->viewport(viewport);
        return WG_OK;
    }

    Status RdgPassContext::bind_vert_buffer(GfxVertBuffer* buffer, int index, int offset) {
        m_cmd_list->bind_vert_buffer(buffer, index, offset);
        return WG_OK;
    }

    Status RdgPassContext::bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) {
        m_cmd_list->bind_index_buffer(buffer, index_type, offset);
        return WG_OK;
    }

    Status RdgPassContext::draw(int vertex_count, int base_vertex, int instance_count) {
        m_cmd_list->draw(vertex_count, base_vertex, instance_count);
        return WG_OK;
    }

    Status RdgPassContext::draw_indexed(int index_count, int base_vertex, int instance_count) {
        m_cmd_list->draw_indexed(index_count, base_vertex, instance_count);
        return WG_OK;
    }

    Status RdgPassContext::dispatch(Vec3i group_count) {
        m_cmd_list->dispatch(group_count);
        return WG_OK;
    }

}// namespace wmoge
