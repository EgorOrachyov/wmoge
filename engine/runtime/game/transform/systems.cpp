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

#include "systems.hpp"

namespace wmoge {

    static void gm_transform_recursive(const Mat4x4f& parent, int frame_id, EcsEntity entity, EcsWorld& world) {
        auto& trsf_frame   = world.get_component_rw<GmTransformFrameComponent>(entity);
        auto& trsf         = world.get_component_rw<GmTransformComponent>(entity);
        auto& children     = world.get_component_rw<GmChildrenComponent>(entity);
        auto& mat_l        = world.get_component_rw<GmMatLocalComponent>(entity);
        auto& mat_l2v      = world.get_component_rw<GmMatLocalToWorldComponent>(entity);
        auto& mat_l2v_prev = world.get_component_rw<GmMatLocalToWorldPrevComponent>(entity);

        Mat4x4f matrix_l   = trsf.t.to_mat4x4();
        Mat4x4f matrix_l2w = parent * matrix_l;

        mat_l2v_prev.m = mat_l2v.m;
        mat_l2v.m      = Math3d::to_m3x4f(matrix_l2w);
        mat_l.m        = Math3d::to_m3x4f(matrix_l);

        trsf_frame.frame    = frame_id;
        trsf_frame.is_dirty = false;

        for (const EcsEntity& child : children.ids) {
            gm_transform_recursive(matrix_l2w, frame_id, child, world);
        }
    }

    void gm_transform_movable_hier_system(int frame_id, EcsQuery<GmTransfromHierAccess>& query) {
        EcsWorld& world = query.get_world();

        query.for_each([&](int entity_idx) {
            auto& parent = query.get_component<GmParentComponent>(entity_idx);

            if (parent.id.is_invalid()) {
                gm_transform_recursive(Math3d::identity(), frame_id, query.get_entity(entity_idx), world);
            }
        });
    }

    void gm_transform_movable_flat_system(int frame_id, EcsQuery<GmTransfromFlatAccess>& query) {
        query.for_each([&](int entity_idx) {
            auto& trsf_frame = query.get_component<GmTransformFrameComponent>(entity_idx);
            if (!trsf_frame.is_dirty) {
                return;
            }

            auto& trsf         = query.get_component<GmTransformComponent>(entity_idx);
            auto& mat_l2v      = query.get_component<GmMatLocalToWorldComponent>(entity_idx);
            auto& mat_l2v_prev = query.get_component<GmMatLocalToWorldPrevComponent>(entity_idx);

            Mat4x4f matrix_l = trsf.t.to_mat4x4();

            mat_l2v_prev.m = mat_l2v.m;
            mat_l2v.m      = Math3d::to_m3x4f(matrix_l);

            trsf_frame.frame    = frame_id;
            trsf_frame.is_dirty = false;
        });
    }

}// namespace wmoge