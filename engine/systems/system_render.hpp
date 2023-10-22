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
#include "render/render_mesh_static.hpp"
#include "render/render_object.hpp"
#include "render/render_scene.hpp"
#include "render/visibility.hpp"
#include "scene/scene_components.hpp"

#include "core/engine.hpp"
#include "render/aux_draw_manager.hpp"

namespace wmoge {

    /**
     * @class EcsSysUpdateStaticMeshes
     * @brief Update static meshes from scene before rendering
     */
    class EcsSysUpdateStaticMeshes final : public EcsSystem {
    public:
        RenderScene*      render_scene = nullptr;
        VisibilitySystem* vis_system   = nullptr;

        void process(EcsWorld& world, const EcsEntity& enity, EcsComponentLocalToWorld& l2w, EcsComponentMeshStatic& mesh_static) {
            if (mesh_static.dirty) {
                const int     id   = mesh_static.mesh->get_primitive_id();
                const Mat4x4f mat  = l2w.matrix.transpose();
                const Aabbf   aabb = mesh_static.mesh->get_aabb().transform(l2w.matrix);

                vis_system->update_item_bbox(mesh_static.vis_item, aabb);

                GPURenderObjectData& gpu_data = render_scene->get_objects_gpu_data()[id];
                gpu_data.LocalToWorld         = mat;
                gpu_data.LocalToWorldPrev     = mat;
                gpu_data.NormalMatrix         = mat;
                gpu_data.AabbPos              = Vec4f(aabb.center(), 0.0f);
                gpu_data.AabbSizeHalf         = Vec4f(aabb.extent(), 0.0f);

                mesh_static.dirty = false;
            }
        }

        void process_batch(class EcsWorld& world, EcsArchStorage& storage, int start_entity, int count) override {
            WG_ECS_SYSTEM_BIND(EcsSysUpdateStaticMeshes);
        }

        EcsSystemExecMode get_exec_mode() const override { return EcsSystemExecMode::OnWorkers; }
        StringId          get_name() const override { return SID("update_static_meshes"); }
        EcsQuery          get_query() const override {
            EcsQuery query;
            query.set_read<EcsComponentLocalToWorld>();
            query.set_read<EcsComponentMeshStatic>();
            return query;
        }
    };

    /**
     * @class EcsSysPocessVisStaticMeshes
     * @brief Process visibility result for static meshes
     */
    class EcsSysPocessVisStaticMeshes final : public EcsSystem {
    public:
        RenderScene*      render_scene = nullptr;
        VisibilitySystem* vis_system   = nullptr;

        void process(EcsWorld& world, const EcsEntity& enity, EcsComponentMeshStatic& mesh_static) {
            const VisibilityItemResult result = vis_system->get_item_result(mesh_static.vis_item);

            if (result.cam_mask.any()) {
                mesh_static.mesh->procces_visibility(result.cam_mask, result.distance);
            }

            render_scene->get_objects_vis()[mesh_static.primitive_id] = result.cam_mask;
        }

        void process_batch(class EcsWorld& world, EcsArchStorage& storage, int start_entity, int count) override {
            WG_ECS_SYSTEM_BIND(EcsSysPocessVisStaticMeshes);
        }

        EcsSystemExecMode get_exec_mode() const override { return EcsSystemExecMode::OnWorkers; }
        StringId          get_name() const override { return SID("process_vis_static_meshes"); }
        EcsQuery          get_query() const override {
            EcsQuery query;
            query.set_read<EcsComponentMeshStatic>();
            return query;
        }
    };

}// namespace wmoge

#endif//WMOGE_SYSTEM_RENDER_HPP