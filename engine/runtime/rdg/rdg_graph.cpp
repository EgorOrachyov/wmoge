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
#include "profiler/profiler.hpp"

#include <cassert>

namespace wmoge {

    RDGGraph::RDGGraph(RDGPool* pool) {
        m_pool = pool;
    }

    RDGPass& RDGGraph::add_pass(Strid name, RDGPassFlags flags) {
        RDGPass& pass = m_passes.emplace_back(*this, name, next_pass_id(), flags);
        return pass;
    }

    RDGPass& RDGGraph::add_compute_pass(Strid name, RDGPassFlags flags) {
        return add_pass(name, flags & RDGPassFlags{RDGPassFlag::ComputePass});
    }

    RDGPass& RDGGraph::add_graphics_pass(Strid name, RDGPassFlags flags) {
        return add_pass(name, flags & RDGPassFlags{RDGPassFlag::GraphicsPass});
    }

    RDGPass& RDGGraph::add_material_pass(Strid name, RDGPassFlags flags) {
        return add_pass(name, flags & RDGPassFlags{RDGPassFlag::MaterialPass});
    }

    RDGTexture* RDGGraph::create_texture(const GfxTextureDesc& desc, Strid name) {
        Ref<RDGTexture> resource = make_ref<RDGTexture>(desc, next_res_id(), name);
        add_resource(resource);
        return resource.get();
    }

    RDGTexture* RDGGraph::import_texture(const GfxTextureRef& texture) {
        if (RDGTexture* r = find_texture(texture)) {
            return r;
        }

        Ref<RDGTexture> resource = make_ref<RDGTexture>(texture, next_res_id());
        add_resource(resource);
        return resource.get();
    }

    RDGTexture* RDGGraph::find_texture(const GfxTextureRef& texture) {
        auto q = m_resources_imported.find(texture.get());
        if (q != m_resources_imported.end()) {
            return dynamic_cast<RDGTexture*>(q->second);
        }
        return nullptr;
    }

    RDGStorageBuffer* RDGGraph::create_storage_buffer(const GfxBufferDesc& desc, Strid name) {
        Ref<RDGStorageBuffer> resource = make_ref<RDGStorageBuffer>(desc, next_res_id(), name);
        add_resource(resource);
        return resource.get();
    }

    RDGStorageBuffer* RDGGraph::import_storage_buffer(const GfxStorageBufferRef& buffer) {
        if (RDGStorageBuffer* r = find_storage_buffer(buffer)) {
            return r;
        }

        Ref<RDGStorageBuffer> resource = make_ref<RDGStorageBuffer>(buffer, next_res_id());
        add_resource(resource);
        return resource.get();
    }

    RDGStorageBuffer* RDGGraph::find_storage_buffer(const GfxStorageBufferRef& buffer) {
        auto q = m_resources_imported.find(buffer.get());
        if (q != m_resources_imported.end()) {
            return dynamic_cast<RDGStorageBuffer*>(q->second);
        }
        return nullptr;
    }

    Status RDGGraph::compile() {
        return Status();
    }

    Status RDGGraph::execute() {
        return Status();
    }

    void RDGGraph::add_resource(const RDGResourceRef& resource) {
        assert(resource->get_id().value == m_resources.size());
        m_resources.push_back(resource);

        if (resource->get_flags().get(RDGResourceFlag::Imported)) {
            m_resources_imported[resource->get_gfx().get()] = resource.get();
        }
    }

    RDGPassId RDGGraph::next_pass_id() {
        auto id = m_next_pass_id;
        ++m_next_pass_id;
        return id;
    }

    RDGResourceId RDGGraph::next_res_id() {
        auto id = m_next_res_id;
        ++m_next_res_id;
        return id;
    }

}// namespace wmoge
