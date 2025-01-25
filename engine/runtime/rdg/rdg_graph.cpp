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
#include "rdg/rdg_profiling.hpp"

#include <cassert>

namespace wmoge {

    RdgGraph::RdgGraph(RdgPool* pool, GfxDriver* driver, ShaderManager* shader_manager, TextureManager* texture_manager) {
        m_pool            = pool;
        m_driver          = driver;
        m_shader_manager  = shader_manager;
        m_texture_manager = texture_manager;
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
        add_resource(resource, GfxAccess::None);
        return resource.get();
    }

    RdgTexture* RdgGraph::import_texture(const GfxTextureRef& texture) {
        if (RdgTexture* r = find_texture(texture)) {
            return r;
        }
        Ref<RdgTexture> resource = make_ref<RdgTexture>(texture, next_res_id());
        add_resource(resource, GfxAccess::TexureSample);
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
        add_resource(resource, GfxAccess::None);
        return resource.get();
    }

    RdgStorageBuffer* RdgGraph::import_storage_buffer(const GfxStorageBufferRef& buffer) {
        if (RdgStorageBuffer* r = find_storage_buffer(buffer)) {
            return r;
        }
        Ref<RdgStorageBuffer> resource = make_ref<RdgStorageBuffer>(buffer, next_res_id());
        add_resource(resource, GfxAccess::BufferRead);
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
        add_resource(resource, GfxAccess::BufferRead);
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
        add_resource(resource, GfxAccess::BufferRead);
        return resource.get();
    }

    RdgIndexBuffer* RdgGraph::find_index_buffer(const GfxIndexBufferRef& buffer) {
        auto q = m_resources_imported.find(buffer.get());
        if (q != m_resources_imported.end()) {
            return dynamic_cast<RdgIndexBuffer*>(q->second);
        }
        return nullptr;
    }

    RdgParamBlock* RdgGraph::create_param_block(const std::function<RdgParamBlockRef(RdgResourceId)>& factory) {
        Ref<RdgParamBlock> resource = factory(next_res_id());
        add_resource(resource, GfxAccess::None);
        return resource.get();
    }

    Ref<Data> RdgGraph::make_upload_data(array_view<const std::uint8_t> buffer) {
        return make_ref<Data>(buffer.data(), buffer.size());
    }

    Ref<ShaderParamBlock> RdgGraph::make_param_block(Shader* shader, std::int16_t space_idx, const Strid& name) {
        assert(shader);
        Ref<ShaderParamBlock> param_block = make_ref<ShaderParamBlock>(*shader, space_idx, name);
        m_param_blocks.push_back(param_block);
        return param_block;
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

    static GfxTexBarrierType rdg_access_to_barrier(GfxAccess access) {
        switch (access) {
            case GfxAccess::TexureSample:
                return GfxTexBarrierType::Sampling;
            case GfxAccess::RenderTarget:
                return GfxTexBarrierType::RenderTarget;
            case GfxAccess::ImageStore:
                return GfxTexBarrierType::Storage;
            case GfxAccess::CopySource:
                return GfxTexBarrierType::CopySource;
            case GfxAccess::CopyDestination:
                return GfxTexBarrierType::CopyDestination;

            default:
                return GfxTexBarrierType::Undefined;
        }
    }

    static void rdg_transition_resource(GfxCmdListRef& cmd_list, GfxAccess src, GfxAccess dst, RdgResource* resource) {
        if (src == dst) {
            return;
        }

        if (resource->is_vertex()) {
            GfxBuffer* buffer = static_cast<RdgVertBuffer*>(resource)->get_buffer();
            cmd_list->barrier_buffers({&buffer, 1});
        }
        if (resource->is_index()) {
            GfxBuffer* buffer = static_cast<RdgIndexBuffer*>(resource)->get_buffer();
            cmd_list->barrier_buffers({&buffer, 1});
        }
        if (resource->is_uniform()) {
            GfxBuffer* buffer = static_cast<RdgUniformBuffer*>(resource)->get_buffer();
            cmd_list->barrier_buffers({&buffer, 1});
        }
        if (resource->is_storage()) {
            GfxBuffer* buffer = static_cast<RdgStorageBuffer*>(resource)->get_buffer();
            cmd_list->barrier_buffers({&buffer, 1});
        }
        if (resource->is_texture()) {
            GfxTexture* texture = static_cast<RdgTexture*>(resource)->get_texture();
            cmd_list->barrier_images({&texture, 1}, rdg_access_to_barrier(src), rdg_access_to_barrier(dst));
        }
    }

    Status RdgGraph::execute(const RdgExecuteOptions& options) {
        WG_PROFILE_CPU_RDG("RdgGraph::execute");

        const int num_passes    = m_next_pass_id.value;
        const int num_resources = m_next_res_id.value;

        std::vector<GfxAccess> resource_states(num_resources);
        for (int i = 0; i < num_resources; i++) {
            resource_states[i] = m_resources[i].src_access;
        }

        GfxCmdListRef cmd_list = m_driver->acquire_cmd_list(GfxQueueType::Graphics);
        WG_PROFILE_GPU_BEGIN(cmd_list);

        for (int i = 0; i < num_resources; i++) {
            RdgResourceRef& resource = m_resources[i].resource;

            if (resource->is_pooled() && !resource->is_allocated()) {
                resource->allocate(*m_pool);
            }
        }

        for (int i = 0; i < num_resources; i++) {
            RdgResourceRef& resource = m_resources[i].resource;

            if (resource->is_pooled() && !resource->is_allocated()) {
                resource->allocate(*m_pool);
            }
        }

        for (int pass_id = 0; pass_id < num_passes; pass_id++) {
            const RdgPass& pass = m_passes[pass_id];
            RdgPassContext context(cmd_list, m_driver, m_shader_manager, this, pass);

            const array_view<const RdgPassResource> pass_resources = pass.get_resources();
            for (int i = 0; i < static_cast<int>(pass_resources.size()); i++) {
                const RdgPassResource& pass_resource  = pass_resources[i];
                RdgResource*           resource       = pass_resource.resource;
                GfxAccess&             current_access = resource_states[pass_resource.resource->get_id()];

                if (resource->is_transitionable()) {
                    rdg_transition_resource(cmd_list, current_access, pass_resource.access, resource);
                    current_access = pass_resource.access;
                }
                if (resource->is_param_block()) {
                    RdgParamBlock* param_block = static_cast<RdgParamBlock*>(resource);
                    param_block->pack();
                    context.validate_param_block(param_block->get_param_block());
                }
            }

            execute_pass(RdgPassId(pass_id), context);
        }

        for (int i = 0; i < num_resources; i++) {
            RdgResourceRef& resource = m_resources[i].resource;

            if (resource->is_pooled() && resource->is_allocated()) {
                resource->release(*m_pool);
            }
        }

        WG_PROFILE_GPU_END(cmd_list);
        m_driver->submit_cmd_list(cmd_list);

        return WG_OK;
    }

    Ref<GfxSampler> RdgGraph::get_sampler(DefaultSampler sampler) {
        return m_texture_manager->get_sampler(sampler);
    }

    Status RdgGraph::execute_pass(RdgPassId pass_id, RdgPassContext& context) {
        WG_PROFILE_CPU_RDG("RdgGraph::execute_pass");

        const RdgPass&     pass      = m_passes[pass_id];
        const RdgPassData& pass_data = m_passes_data[pass_id];

        for (const RdgEventId event_id : pass_data.events_to_begin) {
            const RdgEvent& event = m_events[event_id];
            ProfilerCpu::instance()->begin_event(&event.mark->mark_cpu, event.data);
            ProfilerGpu::instance()->begin_event(&event.mark->mark_gpu, event.data, context.get_cmd_list());
        }

        Status status;
        {
            WG_PROFILE_GPU_SCOPE_WITH_DESC("RdgGraph::execute_pass", context.get_cmd_list(), pass.get_name().str());

            context.get_cmd_list()->begin_label(pass.get_name());

            if (!pass.is_manual() && pass.is_graphics()) {
                context.begin_render_pass();
            }

            status = pass.get_callback()(context);

            if (!pass.is_manual() && pass.is_graphics()) {
                context.end_render_pass();
            }

            context.get_cmd_list()->end_label();
        }

        for (int i = 0; i < pass_data.events_to_end; i++) {
            ProfilerGpu::instance()->end_event(context.get_cmd_list());
            ProfilerCpu::instance()->end_event();
        }

        return status;
    }

    void RdgGraph::add_resource(const RdgResourceRef& resource, GfxAccess src_access, GfxAccess dst_access) {
        assert(resource->get_id().value == m_resources.size());
        auto& resource_data      = m_resources.emplace_back();
        resource_data.resource   = resource;
        resource_data.src_access = src_access;
        resource_data.dst_access = dst_access;

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

}// namespace wmoge
