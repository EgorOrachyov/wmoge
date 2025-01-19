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
        RdgPass&     pass      = m_passes.emplace_back(*this, name, next_pass_id(), flags);
        RdgPassData& pass_data = m_passes_data.emplace_back();

        pass_data.events_to_begin = m_events_stack;
        m_events_stack.clear();

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

    RdgIndexBuffer* RdgGraph::import_index_buffer(const GfxIndexBufferRef& buffer) {
        if (RdgIndexBuffer* r = find_index_buffer(buffer)) {
            return r;
        }
        Ref<RdgIndexBuffer> resource = make_ref<RdgIndexBuffer>(buffer, next_res_id());
        add_resource(resource);
        return resource.get();
    }

    RdgIndexBuffer* RdgGraph::find_index_buffer(const GfxIndexBufferRef& buffer) {
        auto q = m_resources_imported.find(buffer.get());
        if (q != m_resources_imported.end()) {
            return dynamic_cast<RdgIndexBuffer*>(q->second);
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
        RdgEventId event_id = static_cast<int>(m_events.size());
        RdgEvent&  event    = m_events.emplace_back();
        event.mark          = mark;
        event.data          = data;
        m_events_stack.push_back(event_id);
    }

    void RdgGraph::pop_event() {
        if (!m_events_stack.empty()) {
            m_events_stack.pop_back();
        } else {
            assert(!m_passes_data.empty());
            m_passes_data.back().events_to_end++;
        }
    }

    Status RdgGraph::compile(const RdgCompileOptions& options) {
        WG_PROFILE_CPU_RDG("RdgGraph::compile");

        return WG_OK;
    }

    Status RdgGraph::execute(const RdgExecuteOptions& options) {
        WG_PROFILE_CPU_RDG("RdgGraph::execute");

        return WG_OK;
    }

    Status RdgGraph::execute_pass(RdgPassId pass_id, RdgPassContext& context) {
        const RdgPass&     pass      = m_passes[pass_id];
        const RdgPassData& pass_data = m_passes_data[pass_id];

        for (const RdgEventId event_id : pass_data.events_to_begin) {
            const RdgEvent& event = m_events[event_id];
            ProfilerCpu::instance()->begin_event(event.mark->mark_cpu.get(), event.data);
            ProfilerGpu::instance()->begin_event(event.mark->mark_gpu.get(), event.data, context.get_cmd_list());
        }

        Status status;
        {
            WG_PROFILE_CPU_RDG("RdgGraph::execute_pass");
            WG_PROFILE_GPU_SCOPE_WITH_DESC("RdgGraph::execute_pass", context.get_cmd_list(), pass.get_name().str());

            status = pass.get_callback()(context);
        }

        for (int i = 0; i < pass_data.events_to_end; i++) {
            ProfilerGpu::instance()->end_event(context.get_cmd_list());
            ProfilerCpu::instance()->end_event();
        }

        return status;
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

    RdgProfileMark::RdgProfileMark(std::string name, Strid category, Strid function, Strid file, std::size_t line)
        : name(std::move(name)), category(category), function(function), file(file), line(line) {
        mark_cpu = std::make_unique<ProfilerCpuMark>(ProfilerCpuMark{this->name, this->category, this->function, this->file, this->line});
        mark_gpu = std::make_unique<ProfilerGpuMark>(ProfilerGpuMark{this->name, this->category, this->function, this->file, this->line});
    }

}// namespace wmoge
