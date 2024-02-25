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

#include "visibility.hpp"

#include "core/string_utils.hpp"
#include "core/task_parallel_for.hpp"
#include "debug/profiler.hpp"

#include <cassert>
#include <mutex>
#include <shared_mutex>

namespace wmoge {

    VisibilityItem VisibilitySystem::alloc_item() {
        if (m_free.empty()) {
            const int curr_capacity = int(m_items.size());
            const int new_capacity  = curr_capacity + ALLOC_BATCH_SIZE;

            m_items.resize(new_capacity);
            m_result.resize(new_capacity);

            for (int i = new_capacity - 1; i >= curr_capacity; i--) {
                m_free.push_back(i);
            }
        }

        assert(!m_free.empty());

        VisibilityItem new_item = m_free.back();
        m_free.pop_back();

        m_items[new_item.id]  = VisibilityItemData();
        m_result[new_item.id] = VisibilityItemResult();

        m_items[new_item.id].id = new_item;

        return new_item;
    }

    void VisibilitySystem::release_item(VisibilityItem item) {
        assert(item.is_valid());

        m_items[item.id]  = VisibilityItemData();
        m_result[item.id] = VisibilityItemResult();

        m_free.push_back(item);
    }

    void VisibilitySystem::update_item_min_dist(const VisibilityItem& item, float min_dist) {
        assert(item.is_valid());

        m_items[item.id].min_dist_2 = min_dist * min_dist;
    }

    void VisibilitySystem::update_item_max_dist(const VisibilityItem& item, float max_dist) {
        assert(item.is_valid());

        m_items[item.id].max_dist_2 = max_dist * max_dist;
    }

    void VisibilitySystem::update_item_bbox(const VisibilityItem& item, const Aabbf& aabbf) {
        assert(item.is_valid());

        m_items[item.id].aabb = aabbf;
    }

    VisibilityItemResult VisibilitySystem::get_item_result(const VisibilityItem& item) {
        assert(item.is_valid());

        return m_result[item.id];
    }

    void VisibilitySystem::cull(const CameraList& cameras) {
        WG_AUTO_PROFILE_RENDER("VisibilitySystem::cull");

        const int total_items = int(m_items.size());
        const int n_cameras   = int(cameras.get_size());

        for (int cam_idx = 0; cam_idx < n_cameras; cam_idx++) {
            const Frustumf frustum = cameras.camera_at(cam_idx).get_frustum();
            const Vec3f    pos     = cameras.camera_at(cam_idx).get_position();

            TaskParallelFor task_cull_camera(SID("cull_cam_" + StringUtils::from_int(cam_idx)), [&](TaskContext&, int id, int) {
                VisibilityItemData&   data   = m_items[id];
                VisibilityItemResult& result = m_result[id];

                const float dist_to_camera2    = Vec3f::distance2(pos, data.aabb.center());
                const bool  is_visible_frustum = frustum.is_inside_or_intersects(data.aabb);
                const bool  is_visible_dist    = data.min_dist_2 <= dist_to_camera2 && dist_to_camera2 <= data.max_dist_2;

                result.cam_mask.set(cam_idx, is_visible_frustum && is_visible_dist);

                if (cam_idx == 0) {
                    result.distance = Math::sqrt(dist_to_camera2);
                }

                return 0;
            });

            task_cull_camera.schedule(total_items, m_task_batch).wait_completed();
        }
    }

}// namespace wmoge