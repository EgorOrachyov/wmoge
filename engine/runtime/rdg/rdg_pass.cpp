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
        m_area                = Rect2i{0, 0, target->get_desc().width, target->get_desc().height};
        return reference(target, GfxAccess::RenderTarget);
    }

    RdgPass& RdgPass::color_target(RdgTexture* target, const Color4f& clear_color) {
        RdgPassColorTarget& t = m_color_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::ClearStore;
        t.color               = clear_color;
        t.clear               = true;
        m_area                = Rect2i{0, 0, target->get_desc().width, target->get_desc().height};
        return reference(target, GfxAccess::RenderTarget);
    }

    RdgPass& RdgPass::depth_target(RdgTexture* target) {
        RdgPassDepthTarget& t = m_depth_target;
        t.resource            = target;
        t.op                  = GfxRtOp::LoadStore;
        m_area                = Rect2i{0, 0, target->get_desc().width, target->get_desc().height};
        return reference(target, GfxAccess::RenderTarget);
    }

    RdgPass& RdgPass::depth_target(RdgTexture* target, float clear_depth, int clear_stencil) {
        RdgPassDepthTarget& t = m_depth_target;
        t.resource            = target;
        t.op                  = GfxRtOp::ClearStore;
        t.depth               = clear_depth;
        t.stencil             = clear_stencil;
        t.clear_depth         = true;
        t.clear_stencil       = true;
        m_area                = Rect2i{0, 0, target->get_desc().width, target->get_desc().height};
        return reference(target, GfxAccess::RenderTarget);
    }

    RdgPass& RdgPass::window_target(const Ref<Window>& window) {
        RdgPassWindowTarget& t = m_window_target;
        t.window               = window;
        m_area                 = Rect2i{0, 0, window->fbo_width(), window->fbo_height()};
        return *this;
    }

    RdgPass& RdgPass::window_target(const Ref<Window>& window, const Color4f& clear_color) {
        RdgPassWindowTarget& t = m_window_target;
        t.window               = window;
        t.color                = clear_color;
        t.clear_color          = true;
        t.op_color             = GfxRtOp::ClearStore;
        m_area                 = Rect2i{0, 0, window->fbo_width(), window->fbo_height()};
        return *this;
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

    RdgPass& RdgPass::params(RdgParamBlock* resource) {
        return reference(resource, GfxAccess::None);
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

        if (has_depth_target()) {
            desc.depth_stencil_fmt = m_depth_target.resource->get_desc().format;
            desc.depth_op          = m_depth_target.op;
            desc.stencil_op        = m_depth_target.op;
        }

        return desc;
    }

    GfxRenderPassDesc RdgPass::make_render_pass_desc(const GfxWindowProps& props) const {
        GfxRenderPassDesc desc;

        desc.color_target_fmts[0] = props.color_format;
        desc.color_target_ops[0]  = m_window_target.op_color;
        desc.depth_stencil_fmt    = props.depth_stencil_format;
        desc.depth_op             = m_window_target.op_depth;
        desc.stencil_op           = m_window_target.op_stencil;

        return desc;
    }

    GfxFrameBufferDesc RdgPass::make_framebuffer_desc(const Ref<GfxRenderPass>& render_pass) const {
        GfxFrameBufferDesc desc;

        for (int i = 0; i < m_color_targets.size(); i++) {
            desc.color_targets[i].texture = GfxTextureRef(m_color_targets[i].resource->get_texture());
            desc.color_targets[i].mip     = m_color_targets[i].mip;
            desc.color_targets[i].slice   = m_color_targets[i].slice;
        }

        if (has_depth_target()) {
            desc.depth_stencil_target.texture = GfxTextureRef(m_depth_target.resource->get_texture());
            desc.depth_stencil_target.mip     = m_depth_target.mip;
            desc.depth_stencil_target.slice   = m_depth_target.slice;
        }

        desc.render_pass = render_pass;

        return desc;
    }

    GfxRenderPassBeginInfo RdgPass::make_render_pass_begin_info(const GfxFrameBufferRef& frame_buffer) const {
        GfxRenderPassBeginInfo info;

        info.name         = get_name();
        info.frame_buffer = frame_buffer;
        info.area         = m_area;

        for (int i = 0; i < m_color_targets.size(); i++) {
            info.clear_color[i] = m_color_targets[i].color;
        }

        if (has_depth_target()) {
            info.clear_depth   = m_depth_target.depth;
            info.clear_stencil = m_depth_target.stencil;
        }

        return info;
    }

    GfxRenderPassWindowBeginInfo RdgPass::make_render_pass_window_begin_info(const Ref<GfxRenderPass>& render_pass) const {
        GfxRenderPassWindowBeginInfo info;

        info.name           = get_name();
        info.render_pass    = render_pass;
        info.area           = m_area;
        info.window         = m_window_target.window;
        info.clear_color[0] = m_window_target.color;
        info.clear_depth    = m_window_target.depth;
        info.clear_stencil  = m_window_target.stencil;

        return info;
    }

    bool RdgPass::is_manual() const {
        return m_flags.get(RdgPassFlag::Manual);
    }

    bool RdgPass::is_graphics() const {
        return m_flags.get(RdgPassFlag::GraphicsPass);
    }

    bool RdgPass::is_window_pass() const {
        return m_window_target.window;
    }

    bool RdgPass::has_depth_target() const {
        return m_depth_target.resource;
    }

    bool RdgPass::has_resource(RdgResource* r) const {
        return m_referenced.find(r) != m_referenced.end();
    }

    RdgPassContext::RdgPassContext(const GfxCmdListRef& cmd_list, GfxDriver* driver, ShaderManager* shader_manager, RdgGraph* graph, const RdgPass& pass)
        : m_cmd_list(cmd_list), m_driver(driver), m_shader_manager(shader_manager), m_graph(graph), m_pass(pass) {

        m_is_color_pass    = m_pass.get_flags().get(RdgPassFlag::GraphicsPass);
        m_use_frame_buffer = m_is_color_pass && !pass.is_window_pass();
        m_use_window       = m_is_color_pass && pass.is_window_pass();
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

    void RdgPassContext::begin_render_pass() {
        assert(m_is_color_pass);
        assert(m_use_frame_buffer || m_use_window);

        if (m_use_frame_buffer) {
            GfxRenderPassDesc rp_desc = m_pass.make_render_pass_desc();
            m_render_pass_ref         = m_driver->make_render_pass(rp_desc);

            GfxFrameBufferDesc fb_desc = m_pass.make_framebuffer_desc(m_render_pass_ref);
            m_frame_buffer_ref         = m_driver->make_frame_buffer(fb_desc);

            GfxRenderPassBeginInfo rp_info = m_pass.make_render_pass_begin_info(m_frame_buffer_ref);
            get_cmd_list()->begin_render_pass(rp_info);
        }

        if (m_use_window) {
            GfxRenderPassDesc rp_desc = m_pass.make_render_pass_desc(m_driver->get_window_props(m_pass.get_window_target().window));
            m_render_pass_ref         = m_driver->make_render_pass(rp_desc);

            GfxRenderPassWindowBeginInfo rp_info = m_pass.make_render_pass_window_begin_info(m_render_pass_ref);
            get_cmd_list()->begin_render_pass(rp_info);
        }
    }

    Status RdgPassContext::bind_param_block(ShaderParamBlock* param_block) {
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

    Status RdgPassContext::bind_pso_graphics(Shader* shader, Strid technique, Strid pass, const buffered_vector<ShaderOptionVariant>& options, const GfxVertAttribs& attribs) {
        auto permutation = shader->permutation(technique, pass, options, attribs);
        if (!permutation) {
            return StatusCode::InvalidParameter;
        }
        return bind_pso_graphics(shader, *permutation, GfxVertElements::make(attribs));
    }

    Status RdgPassContext::bind_pso_compute(Shader* shader, const ShaderPermutation& permutation) {
        GfxPsoComputeRef pso = m_shader_manager->get_or_create_pso_compute(shader, permutation);
        if (!pso) {
            return StatusCode::NoValue;
        }
        m_cmd_list->bind_pso(pso);
        return WG_OK;
    }

    Status RdgPassContext::bind_pso_compute(Shader* shader, Strid technique, Strid pass, const buffered_vector<ShaderOptionVariant>& options) {
        auto permutation = shader->permutation(technique, pass, options);
        if (!permutation) {
            return StatusCode::InvalidParameter;
        }
        return bind_pso_compute(shader, *permutation);
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

    void RdgPassContext::end_render_pass() {
        assert(m_is_color_pass);
        assert(m_use_frame_buffer || m_use_window);

        if (m_is_color_pass) {
            get_cmd_list()->end_render_pass();
        }
    }

}// namespace wmoge
