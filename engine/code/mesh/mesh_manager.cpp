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

#include "mesh_manager.hpp"

#include "core/ioc_container.hpp"
#include "gfx/gfx_driver.hpp"
#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"

#include <cassert>

namespace wmoge {

    MeshManager::MeshManager(IocContainer* ioc) {
        m_gfx_driver = ioc->resolve_value<GfxDriver>();
        m_callback   = std::make_shared<Mesh::Callback>([this](Mesh* mesh) { remove_mesh(mesh); });
    }

    Ref<Mesh> MeshManager::create_mesh(MeshDesc& desc) {
        WG_PROFILE_CPU_MESH("MeshManager::create_mesh");
        std::lock_guard guard(m_mutex);

        desc.flags |= {MeshFlag::Managed};
        auto mesh = make_ref<Mesh>(desc);
        init_mesh(mesh.get());
        return mesh;
    }

    void MeshManager::add_mesh(const Ref<Mesh>& mesh) {
        assert(mesh);
        assert(!has_mesh(mesh.get()));

        std::lock_guard guard(m_mutex);

        mesh->set_mesh_callback(m_callback);
        m_meshes[mesh.get()].weak_ref = mesh;
    }

    void MeshManager::remove_mesh(Mesh* mesh) {
        assert(mesh);
        assert(has_mesh(mesh));

        std::lock_guard guard(m_mutex);

        auto entry = m_meshes.find(mesh);
        delete_mesh(mesh);
        m_meshes.erase(entry);
    }

    void MeshManager::queue_mesh_upload(Mesh* mesh) {
        assert(mesh);
        assert(has_mesh(mesh));

        std::lock_guard guard(m_mutex);
        m_meshes[mesh].state.set(State::PendingUpload);
    }

    bool MeshManager::has_mesh(Mesh* mesh) {
        assert(mesh);

        std::lock_guard guard(m_mutex);
        return m_meshes.find(mesh) != m_meshes.end();
    }

    void MeshManager::flush_meshes_upload() {
        WG_PROFILE_CPU_MESH("MeshManager::flush_meshes_upload");

        std::lock_guard guard(m_mutex);

        std::vector<Mesh*> for_upload;
        for (auto& iter : m_meshes) {
            auto& entry = iter.second;

            if (entry.state.get(State::PendingUpload)) {
                entry.state.set(State::PendingUpload, false);
                for_upload.push_back(iter.first);
            }
        }

        if (for_upload.empty()) {
            return;
        }

        std::vector<GfxBuffer*> for_barrier;
        for (Mesh* mesh : for_upload) {
            array_view<const Ref<GfxVertBuffer>> gfx_vb = mesh->get_gfx_vertex_buffers();
            for (auto& buffer : gfx_vb) {
                for_barrier.push_back(buffer.get());
            }

            array_view<const Ref<GfxIndexBuffer>> gfx_ib = mesh->get_gfx_index_buffers();
            for (auto& buffer : gfx_ib) {
                for_barrier.push_back(buffer.get());
            }
        }

        auto cmd = m_gfx_driver->acquire_cmd_list(GfxQueueType::Graphics);
        WG_PROFILE_GPU_BEGIN(cmd);
        {
            WG_PROFILE_GPU_SCOPE("MeshManager::flust_meshes_upload", cmd);

            cmd->barrier_buffers(for_barrier);
            for (Mesh* mesh : for_upload) {
                upload_mesh(mesh, cmd);
            }
            cmd->barrier_buffers(for_barrier);
        }
        WG_PROFILE_GPU_END(cmd);
        m_gfx_driver->submit_cmd_list(cmd);

        WG_LOG_INFO("uploaded " << for_upload.size() << " meshes to gpu");
    }

    void MeshManager::init_mesh(Mesh* mesh) {
        assert(mesh);

        auto& entry    = m_meshes[mesh];
        entry.weak_ref = WeakRef<Mesh>(mesh);

        array_view<const Ref<Data>>     vb = mesh->get_vertex_buffers();
        std::vector<Ref<GfxVertBuffer>> gfx_vb(vb.size());
        for (std::size_t i = 0; i < vb.size(); i++) {
            gfx_vb[i] = m_gfx_driver->make_vert_buffer(static_cast<int>(vb[i]->size()), mesh->get_mem_usage(), SIDDBG(mesh->get_name().str() + " vert_buffer i=" + std::to_string(i)));
        }
        mesh->set_gfx_vertex_buffers(std::move(gfx_vb));

        array_view<const Ref<Data>>      ib = mesh->get_index_buffers();
        std::vector<Ref<GfxIndexBuffer>> gfx_ib(ib.size());
        for (std::size_t i = 0; i < ib.size(); i++) {
            gfx_ib[i] = m_gfx_driver->make_index_buffer(static_cast<int>(ib[i]->size()), mesh->get_mem_usage(), SIDDBG(mesh->get_name().str() + " index_buffer i=" + std::to_string(i)));
        }
        mesh->set_gfx_index_buffers(std::move(gfx_ib));

        mesh->set_mesh_callback(m_callback);
    }

    void MeshManager::delete_mesh(Mesh* mesh) {
        assert(mesh);

        mesh->release_gfx_buffers();
    }

    void MeshManager::upload_mesh(Mesh* mesh, const GfxCmdListRef& cmd) {
        WG_PROFILE_CPU_MESH("MeshManager::upload_mesh");
        WG_PROFILE_GPU_SCOPE("MeshManager::upload_mesh", cmd);

        array_view<const Ref<Data>>          vb     = mesh->get_vertex_buffers();
        array_view<const Ref<GfxVertBuffer>> gfx_vb = mesh->get_gfx_vertex_buffers();
        {
            WG_PROFILE_GPU_SCOPE("upload_vert_buffers", cmd.get());

            for (std::size_t i = 0; i < vb.size(); i++) {
                cmd->update_vert_buffer(gfx_vb[i], 0, static_cast<int>(vb[i]->size()), {vb[i]->buffer(), vb[i]->size()});
            }
        }

        array_view<const Ref<Data>>           ib     = mesh->get_index_buffers();
        array_view<const Ref<GfxIndexBuffer>> gfx_ib = mesh->get_gfx_index_buffers();
        {
            WG_PROFILE_GPU_SCOPE("upload_index_buffers", cmd.get());

            for (std::size_t i = 0; i < ib.size(); i++) {
                cmd->update_index_buffer(gfx_ib[i], 0, static_cast<int>(ib[i]->size()), {ib[i]->buffer(), ib[i]->size()});
            }
        }
    }

}// namespace wmoge