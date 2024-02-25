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

#include "core/fast_map.hpp"
#include "core/synchronization.hpp"
#include "render/render_queue.hpp"

#include <atomic>
#include <vector>

namespace wmoge {

    /**
     * @class MeshBucketKey
     * @brief Mesh bucket key to group render cmd for dynamic merge of draw call
     */
    struct MeshBucketKey final {
        RenderCmd cmd;//< Template cmd for instancing

        [[nodiscard]] bool        operator==(const MeshBucketKey& other) const;
        [[nodiscard]] std::size_t hash() const;
    };

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::MeshBucketKey> {
        std::size_t operator()(const wmoge::MeshBucketKey& key) const {
            return key.hash();
        }
    };

}// namespace std

namespace wmoge {

    /**
     * @class MeshBucketSlot
     * @brief Bucket slot stores data to build instanced render cmd for draw call
     */
    struct MeshBucketSlot final {
        RenderCmd            cmd;       //< Template cmd for instancing
        std::atomic_uint32_t num{0};    //< Num of external references to cmd
        int                  index = -1;//< Bucket slot unique index for look-up
    };

    /**
     * @class MeshBucketMap
     * @brief Thread-safe map to assign render cmd bucket slots for merging for dynamic instansing
     * 
     * Maps uniques render cmds to bucket slots for instancing. Cmds having the same bucket slot
     * are sorted together for ruther merging.
     */
    class MeshBucketMap final {
    public:
        void                           add_for_instancing(const RenderCmd& cmd, RenderCmd*& cmd_template, int& bucket_slot);
        void                           reset();
        std::optional<MeshBucketSlot*> find_slot(const RenderCmd& cmd);
        MeshBucketSlot*                find_insert_slot(const RenderCmd& cmd);

        [[nodiscard]] MeshBucketSlot& get_slot(int idx) { return *m_slots[idx]; }
        [[nodiscard]] int             get_slots_count() { return int(m_slots.size()); }

    private:
        fast_map<MeshBucketKey, int>                 m_buckets_map;
        std::vector<std::unique_ptr<MeshBucketSlot>> m_slots;
        RwMutexWritePrefer                           m_mutex;
    };

    /**
     * @class MeshRenderCmdMerger
     * @brief Performs merging of sorted draw cmds accordingly to assigned bucket slots
     * 
     * Traverses sorted queues and merges consecutive cmds with the same bucket slot index.
     * When traverses and merges, pathces cmds, adding prmitive ids per-inst vertex attribute
     * to fetch data on the gpu from the per-frame global storage.
     */
    class MeshRenderCmdMerger final {
    public:
        MeshRenderCmdMerger() = default;

        Status proccess_queue(RenderQueue& queue);
        void   set_scene(class RenderScene* scene);
        void   set_cmd_allocator(class RenderCmdAllocator& allocator);
        void   clear();

    private:
        class RenderCmdAllocator* m_cmd_allocator = nullptr;
        class RenderScene*        m_scene         = nullptr;
        std::atomic_uint32_t      m_primitive_offset{0};
    };

}// namespace wmoge