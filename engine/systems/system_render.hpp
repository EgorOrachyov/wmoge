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

#ifndef WMOGE_SYSTEM_RENDER_HPP
#define WMOGE_SYSTEM_RENDER_HPP

#include "ecs/ecs_system.hpp"
#include "ecs/ecs_world.hpp"
#include "render/render_object.hpp"
#include "scene/scene_components.hpp"

namespace wmoge {

    /**
     * @class EcsSysCollectStaticMeshes
     * @brief Collect static mesh from scene for rendering
     */
    class EcsSysCollectStaticMeshes final : public EcsSystem {
    public:
        RenderObjectCollector* object_collector = nullptr;

        void process(EcsWorld& world, const EcsEntity& enity, EcsComponentLocalToWorld& l2w, EcsComponentMeshStatic& mesh_static) {
            mesh_static.mesh->update_transform(l2w.matrix);
            object_collector->add(mesh_static.mesh.get());
        }

        void process_batch(class EcsWorld& world, EcsArchStorage& storage, int start_entity, int count) override {
            WG_ECS_SYSTEM_BIND(EcsSysCollectStaticMeshes);
        }

        EcsSystemExecMode get_exec_mode() const override { return EcsSystemExecMode::OnWorkers; }
        StringId          get_name() const override { return SID("collect_static_meshes"); }
        EcsQuery          get_query() const override {
            EcsQuery query;
            query.set_read<EcsComponentLocalToWorld>();
            query.set_read<EcsComponentMeshStatic>();
            return query;
        }
    };

}// namespace wmoge

#endif//WMOGE_SYSTEM_RENDER_HPP