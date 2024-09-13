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

#include "gfx/gfx_driver.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <cassert>

namespace wmoge {

    MeshManager::MeshManager() {
        m_gfx_driver = IocContainer::iresolve_v<GfxDriver>();
        m_callback   = std::make_shared<Mesh::Callback>([this](Mesh* mesh) { remove_mesh(mesh); });
    }

    Ref<Mesh> MeshManager::create_mesh(MeshFlags flags) {
        flags.set(MeshFlag::Managed);

        Ref<Mesh> mesh = make_ref<Mesh>(flags);
        add_mesh(mesh);
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
        if (entry->second.state.get(MeshState::Inited)) {
            delete_gfx_resource(mesh);
        }
        m_meshes.erase(entry);
    }

    void MeshManager::init_mesh(Mesh* mesh) {
        assert(mesh);
        assert(has_mesh(mesh));

        create_gfx_resource(mesh);

        std::lock_guard guard(m_mutex);
        m_meshes[mesh].state.set(MeshState::Inited);
        m_meshes[mesh].state.set(MeshState::PendingUpload);
    }

    bool MeshManager::has_mesh(Mesh* mesh) {
        std::lock_guard guard(m_mutex);
        return m_meshes.find(mesh) != m_meshes.end();
    }

    void MeshManager::create_gfx_resource(Mesh* mesh) {
    }

    void MeshManager::delete_gfx_resource(Mesh* mesh) {
    }

    void MeshManager::upload_gfx_data(Mesh* mesh, GfxCmdListRef& cmd_list) {
    }

}// namespace wmoge