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

#include "core/array_view.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "math/aabb.hpp"
#include "math/frustum.hpp"
#include "render/camera.hpp"
#include "render/render_defs.hpp"

#include <atomic>
#include <vector>

namespace wmoge {

    /**
     * @class CullingItem
     * @brief Id of the item to reference in visibility system 
     */
    struct CullingItem {
        CullingItem() = default;
        CullingItem(int id) : id(id) {}

        bool operator==(const CullingItem& other) const { return id == other.id; }
        bool operator!=(const CullingItem& other) const { return id != other.id; }

        bool is_valid() const { return id != -1; }

        operator int() const { return id; }
        operator bool() const { return is_valid(); }

        [[nodiscard]] std::string to_string() const { return StringUtils::from_int(id); }

        int id = -1;
    };

    /**
     * @class CullingItemData
     * @brief Data of a single item for management
     */
    struct CullingItemData {
        Aabbf       aabb;
        float       min_dist_2 = 0.0f;
        float       max_dist_2 = 10000000000.0f;
        CullingItem id;
    };

    /**
     * @class CullingItemResult
     * @brief Result of a visibility item culling tests
     */
    struct CullingItemResult {
        RenderCameraMask cam_mask;
        float            distance;
    };

    /**
     * @class CullingManager
     * @brief Manages allocation, frustum and occlussion culling of visibility items
     */
    class CullingManager {
    public:
        static constexpr int ALLOC_BATCH_SIZE = 1024;

        CullingManager()                      = default;
        CullingManager(const CullingManager&) = delete;
        CullingManager(CullingManager&&)      = delete;

        CullingItem       alloc_item();
        void              release_item(CullingItem item);
        void              update_item_min_dist(const CullingItem& item, float min_dist);
        void              update_item_max_dist(const CullingItem& item, float max_dist);
        void              update_item_bbox(const CullingItem& item, const Aabbf& aabbf);
        CullingItemResult get_item_result(const CullingItem& item);

        void cull(const CameraList& cameras);

    private:
        std::vector<CullingItemData>   m_items;
        std::vector<CullingItemResult> m_result;
        std::vector<int>               m_free;
        int                            m_task_batch = 16;
    };

}// namespace wmoge