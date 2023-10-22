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

#ifndef WMGOE_VISIBILITY_HPP
#define WMGOE_VISIBILITY_HPP

#include "core/array_view.hpp"
#include "core/string_id.hpp"
#include "core/synchronization.hpp"
#include "core/unrolled_list.hpp"
#include "math/aabb.hpp"
#include "math/frustum.hpp"
#include "render/render_camera.hpp"
#include "render/render_defs.hpp"

#include <atomic>
#include <vector>

namespace wmoge {

    /** @brief Id of the item to reference in visibility system */
    using VisibilityItem = int;

    /** @brief Id of invalid item */
    static constexpr VisibilityItem VIS_ITEM_INVALID = -1;

    /**
     * @class VisibilityItemData
     * @brief Data of a single item for management
     */
    struct VisibilityItemData {
        Aabbf          aabb;
        float          min_dist_2 = 0.0f;
        float          max_dist_2 = 10000000000.0f;
        VisibilityItem id         = VIS_ITEM_INVALID;
    };

    /**
     * @class VisibilityItemResult
     * @brief Result of a visibility item culling tests
     */
    struct VisibilityItemResult {
        RenderCameraMask cam_mask;
        float            distance;
    };

    /**
     * @class VisibilitySystem
     * @brief Manages allocation, frustum and occlussion culling of visibility items
     */
    class VisibilitySystem {
    public:
        static constexpr int ALLOC_BATCH_SIZE = 1024;

        VisibilitySystem()                        = default;
        VisibilitySystem(const VisibilitySystem&) = delete;
        VisibilitySystem(VisibilitySystem&&)      = delete;

        VisibilityItem       alloc_item();
        void                 release_item(VisibilityItem item);
        void                 update_item_min_dist(const VisibilityItem& item, float min_dist);
        void                 update_item_max_dist(const VisibilityItem& item, float max_dist);
        void                 update_item_bbox(const VisibilityItem& item, const Aabbf& aabbf);
        VisibilityItemResult get_item_result(const VisibilityItem& item);

        void cull(const RenderCameras& cameras);

    private:
        std::vector<VisibilityItemData>   m_items;
        std::vector<VisibilityItemResult> m_result;
        std::vector<int>                  m_free;
        int                               m_task_batch = 16;

        RwMutexWritePrefer m_mutex;
    };

}// namespace wmoge

#endif//WMGOE_VISIBILITY_HPP