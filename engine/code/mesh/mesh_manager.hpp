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

#pragma once

#include "core/flat_map.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_driver.hpp"
#include "mesh/mesh.hpp"

#include <functional>
#include <memory>
#include <mutex>

namespace wmoge {

    /**
     * @class MeshManager
     * @brief Manager for gpu meshes for rendering
     * 
     * Manages gpu meshes allocation, data upload and update.
     */
    class MeshManager {
    public:
        MeshManager(class IocContainer* ioc);

        Ref<Mesh> create_mesh(MeshDesc desc);
        void      add_mesh(const Ref<Mesh>& mesh);
        void      remove_mesh(Mesh* mesh);
        void      queue_mesh_upload(Mesh* mesh);
        bool      has_mesh(Mesh* mesh);
        void      flush_meshes_upload();

    private:
        void init_mesh(Mesh* mesh);
        void delete_mesh(Mesh* mesh);
        void upload_mesh(Mesh* mesh, const GfxCmdListRef& cmd);

    private:
        enum class State {
            PendingUpload = 0
        };

        struct Entry {
            WeakRef<Mesh> weak_ref;
            Mask<State>   state;
        };

        flat_map<Mesh*, Entry> m_meshes;
        Mesh::CallbackRef      m_callback;

        GfxDriver* m_gfx_driver;

        mutable std::mutex m_mutex;
    };

}// namespace wmoge
