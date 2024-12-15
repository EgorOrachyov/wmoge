#include "rdg_graph.hpp"
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

#include "rdg_graph.hpp"

#include "core/log.hpp"
#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"

#include <cassert>

namespace wmoge {

    RdgGraph::RdgGraph(RdgPool* pool, GfxDriver* driver, ShaderManager* shader_manager) {
        m_pool           = pool;
        m_driver         = driver;
        m_shader_manager = shader_manager;
    }

    RdgPass& RdgGraph::add_pass(Strid name, RdgPassFlags flags) {
        RdgPass& pass = m_passes.emplace_back(*this, name, next_pass_id(), flags);
        return pass;
    }

    RdgPass& RdgGraph::add_compute_pass(Strid name, RdgPassFlags flags) {
        return add_pass(name, flags | RdgPassFlags{RdgPassFlag::ComputePass});
    }

    RdgPass& RdgGraph::add_graphics_pass(Strid name, RdgPassFlags flags) {
        return add_pass(name, flags | RdgPassFlags{RdgPassFlag::GraphicsPass});
    }

    RdgPass& RdgGraph::add_material_pass(Strid name, RdgPassFlags flags) {
        return add_pass(name, flags | RdgPassFlags{RdgPassFlag::MaterialPass});
    }

    RdgPass& RdgGraph::add_copy_pass(Strid name, RdgPassFlags flags) {
        return add_pass(name, flags | RdgPassFlags{RdgPassFlag::CopyPass});
    }

    RdgTexture* RdgGraph::create_texture(const GfxTextureDesc& desc, Strid name) {
        Ref<RdgTexture> resource = make_ref<RdgTexture>(desc, next_res_id(), name);
        add_resource(resource);
        return resource.get();
    }

    RdgTexture* RdgGraph::import_texture(const GfxTextureRef& texture) {
        if (RdgTexture* r = find_texture(texture)) {
            return r;
        }

        Ref<RdgTexture> resource = make_ref<RdgTexture>(texture, next_res_id());
        add_resource(resource);
        return resource.get();
    }

    RdgTexture* RdgGraph::find_texture(const GfxTextureRef& texture) {
        auto q = m_resources_imported.find(texture.get());
        if (q != m_resources_imported.end()) {
            return dynamic_cast<RdgTexture*>(q->second);
        }
        return nullptr;
    }

    RdgStorageBuffer* RdgGraph::create_storage_buffer(const GfxBufferDesc& desc, Strid name) {
        Ref<RdgStorageBuffer> resource = make_ref<RdgStorageBuffer>(desc, next_res_id(), name);
        add_resource(resource);
        return resource.get();
    }

    RdgStorageBuffer* RdgGraph::import_storage_buffer(const GfxStorageBufferRef& buffer) {
        if (RdgStorageBuffer* r = find_storage_buffer(buffer)) {
            return r;
        }
        Ref<RdgStorageBuffer> resource = make_ref<RdgStorageBuffer>(buffer, next_res_id());
        add_resource(resource);
        return resource.get();
    }

    RdgStorageBuffer* RdgGraph::find_storage_buffer(const GfxStorageBufferRef& buffer) {
        auto q = m_resources_imported.find(buffer.get());
        if (q != m_resources_imported.end()) {
            return dynamic_cast<RdgStorageBuffer*>(q->second);
        }
        return nullptr;
    }

    RdgVertBuffer* RdgGraph::import_vert_buffer(const GfxVertBufferRef& buffer) {
        if (RdgVertBuffer* r = find_vert_buffer(buffer)) {
            return r;
        }
        Ref<RdgVertBuffer> resource = make_ref<RdgVertBuffer>(buffer, next_res_id());
        add_resource(resource);
        return resource.get();
    }

    RdgVertBuffer* RdgGraph::find_vert_buffer(const GfxVertBufferRef& buffer) {
        auto q = m_resources_imported.find(buffer.get());
        if (q != m_resources_imported.end()) {
            return dynamic_cast<RdgVertBuffer*>(q->second);
        }
        return nullptr;
    }

    Ref<Data> RdgGraph::make_upload_data(array_view<const std::uint8_t> buffer) {
        return make_ref<Data>(buffer.data(), buffer.size());
    }

    Ref<ShaderParamBlock> RdgGraph::make_param_block(Shader* shader, std::int16_t space_idx, const Strid& name) {
        assert(shader);
        return make_ref<ShaderParamBlock>(*shader, space_idx, name);
    }

    void RdgGraph::push_event(RdgProfileMark* mark, const std::string& data) {
        Event& event = m_events.emplace_back();
        event.mark   = mark;
        event.data   = data;
    }

    void RdgGraph::pop_event() {
        m_events.pop_back();
    }

    Status RdgGraph::compile() {
        return Status();
    }

    Status RdgGraph::execute() {
        return Status();
    }

    void RdgGraph::add_resource(const RdgResourceRef& resource) {
        assert(resource->get_id().value == m_resources.size());
        m_resources.push_back(resource);

        if (resource->get_flags().get(RdgResourceFlag::Imported)) {
            m_resources_imported[resource->get_gfx().get()] = resource.get();
        }
    }

    RdgPassId RdgGraph::next_pass_id() {
        auto id = m_next_pass_id;
        ++m_next_pass_id;
        return id;
    }

    RdgResourceId RdgGraph::next_res_id() {
        auto id = m_next_res_id;
        ++m_next_res_id;
        return id;
    }

    RdgProfileScope::RdgProfileScope(RdgProfileMark& mark, const std::string& data, RdgGraph& graph) : graph(graph) {
        graph.push_event(&mark, data);
    }

    RdgProfileScope::~RdgProfileScope() {
        graph.pop_event();
    }

}// namespace wmoge
