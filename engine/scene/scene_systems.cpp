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

#include "scene_systems.hpp"

#include "render/culling.hpp"
#include "scene/scene.hpp"

#include <cassert>

namespace wmoge {

    void EcsSysUpdateHier::process(EcsWorld&                 world,
                                   const EcsEntity&          entity,
                                   const EcsComponentParent& parent, const EcsComponentChildren& children,
                                   const EcsComponentTransform& transform, EcsComponentTransformUpd& transform_upd,
                                   EcsComponentLocalToWorld& l2w, EcsComponentWorldToLocal& w2l) {
        if (!transform_upd.is_dirty) {
            return;
        }
        if (transform_upd.batch_id != current_batch) {
            return;
        }

        Mat4x4f l2w_parent = Math3d::identity();
        Mat4x4f w2l_parent = Math3d::identity();

        if (parent.parent.is_valid()) {
            EcsArch arch = world.get_arch(parent.parent);

            if (arch.has_component<EcsComponentLocalToWorld>()) {
                l2w_parent = world.get_component<EcsComponentLocalToWorld>(parent.parent).matrix;
            }
            if (arch.has_component<EcsComponentWorldToLocal>()) {
                w2l_parent = world.get_component<EcsComponentWorldToLocal>(parent.parent).matrix;
            }
        }

        const Transform3d& t     = transform.transform;
        Transform3d        t_inv = t.inv();

        l2w.matrix = l2w_parent * t.to_mat4x4();
        w2l.matrix = t_inv.to_mat4x4() * w2l_parent;

        num_updated.fetch_add(1);

        for (const EcsEntity child : children.children) {
            if (world.has_component<EcsComponentTransformUpd>(child)) {
                world.get_component_rw<EcsComponentTransformUpd>(child).is_dirty = true;
                num_dirty.fetch_add(1);
            }
        }

        transform_upd.is_dirty           = false;
        transform_upd.last_frame_updated = frame_id;
    }

    void EcsSysReleaseCullItem::process(EcsWorld& world, const EcsEntity& entity, EcsComponentCullingItem& culling_item) {
        if (!culling_item.item.is_valid()) {
            return;
        }

        Scene&          scene           = world.get_attribute<Scene>();
        CullingManager* culling_manager = scene.get_culling_manager();

        culling_manager->release_item(culling_item.item);

        culling_item.item = CullingItem();
    }

}// namespace wmoge