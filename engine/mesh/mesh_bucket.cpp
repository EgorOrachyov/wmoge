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

#include "mesh_bucket.hpp"

#include "core/crc32.hpp"
#include "render/render_scene.hpp"

#include <cassert>
#include <functional>
#include <mutex>
#include <shared_mutex>

namespace wmoge {

    bool MeshBucketKey::operator==(const MeshBucketKey& other) const {
        const RenderCmd& this_cmd  = cmd;
        const RenderCmd& other_cmd = other.cmd;

        if (!(this_cmd.pipeline == other_cmd.pipeline)) {
            return false;
        }
        if (!(this_cmd.call_params == other_cmd.call_params)) {
            return false;
        }
        if (!(this_cmd.vert_buffers == other_cmd.vert_buffers)) {
            return false;
        }
        if (!(this_cmd.index_setup == other_cmd.index_setup)) {
            return false;
        }
        for (int i = 0; i < RenderCmd::NUM_DESC_SETS; i++) {
            if (this_cmd.desc_sets[i] != other_cmd.desc_sets[i]) {
                return false;
            }
            if (this_cmd.desc_sets_slots[i] != other_cmd.desc_sets_slots[i]) {
                return false;
            }
        }

        return true;
    }

    std::size_t MeshBucketKey::hash() const {
        Crc32Hash hash = 0xffffffff;

        hash ^= Crc32::hash(&cmd.pipeline, sizeof(GfxPsoGraphics*));

        for (int i = 0; i < RenderCmd::NUM_DESC_SETS; i++) {
            hash ^= Crc32::hash(&cmd.desc_sets[i], sizeof(GfxDescSet*));
            hash ^= Crc32::hash(&cmd.desc_sets_slots[i], sizeof(int));
        }

        for (int i = 0; i < GfxLimits::MAX_VERT_BUFFERS; i++) {
            hash ^= Crc32::hash(&cmd.vert_buffers.buffers[i], sizeof(GfxVertBuffer*));
            hash ^= Crc32::hash(&cmd.vert_buffers.offsets[i], sizeof(int));
        }

        hash ^= Crc32::hash(&cmd.index_setup.buffer, sizeof(GfxIndexBuffer*));
        hash ^= Crc32::hash(&cmd.index_setup.offset, sizeof(int));
        hash ^= Crc32::hash(&cmd.index_setup.index_type, sizeof(GfxIndexType));

        hash ^= Crc32::hash(&cmd.call_params.base, sizeof(int));
        hash ^= Crc32::hash(&cmd.call_params.count, sizeof(int));
        hash ^= Crc32::hash(&cmd.call_params.instances, sizeof(int));

        return hash;
    }

    void MeshBucketMap::add_for_instancing(const RenderCmd& cmd, RenderCmd*& cmd_template, int& bucket_slot) {
        MeshBucketSlot* slot = find_insert_slot(cmd);
        cmd_template         = &slot->cmd;
        bucket_slot          = slot->index;
    }

    void MeshBucketMap::reset() {
        for (std::unique_ptr<MeshBucketSlot>& slot : m_slots) {
            slot->num.store(0);
        }
    }

    std::optional<MeshBucketSlot*> MeshBucketMap::find_slot(const RenderCmd& cmd) {
        std::shared_lock read_lock(m_mutex);

        MeshBucketKey key;
        key.cmd = cmd;

        auto iter = m_buckets_map.find(key);
        if (iter != m_buckets_map.end()) {
            return m_slots[iter->second].get();
        }

        return std::nullopt;
    }

    MeshBucketSlot* MeshBucketMap::find_insert_slot(const RenderCmd& cmd) {
        std::optional<MeshBucketSlot*> slot_index = find_slot(cmd);

        if (slot_index.has_value()) {
            return slot_index.value();
        }

        std::unique_lock write_lock(m_mutex);

        MeshBucketKey key;
        key.cmd = cmd;

        auto iter = m_buckets_map.find(key);
        if (iter != m_buckets_map.end()) {
            return m_slots[iter->second].get();
        }

        int new_slot_index = int(m_slots.size());
        m_buckets_map[key] = new_slot_index;

        m_slots.resize(m_slots.size() + 1);
        std::unique_ptr<MeshBucketSlot>& new_slot = m_slots.back();

        new_slot        = std::make_unique<MeshBucketSlot>();
        new_slot->cmd   = cmd;
        new_slot->index = new_slot_index;

        return m_slots[new_slot_index].get();
    }

    Status MeshRenderCmdMerger::proccess_queue(RenderQueue& queue) {
        const int queue_size = int(queue.get_size());
        int       idx_read   = 0;
        int       idx_write  = 0;

        std::vector<SortableRenderCmd>& cmds                  = queue.get_queue();
        GfxVector<int, GfxVertBuffer>&  primitives_ids        = m_scene->get_objects_ids();
        GfxVertBuffer*                  primitives_ids_buffer = primitives_ids.get_buffer().get();

        const std::size_t primitives_ids_capacity = primitives_ids.get_size();

        while (idx_read < queue_size) {
            SortableRenderCmd curr_cmd = cmds[idx_read];

            // Traverse queue while consecuetive commands match the instancing slot
            if (curr_cmd.bucket_slot != -1) {
                const int curr_bucket_slot = curr_cmd.bucket_slot;
                int       num_merged       = 1;
                int       idx_first        = idx_read;

                // Determine range
                while ((idx_read + 1) < queue_size && (cmds[idx_read + 1].bucket_slot == curr_bucket_slot)) {
                    idx_read += 1;
                    num_merged += 1;
                }

                // Allocate range of vertex data for per-instance id
                const std::uint32_t id_offset = m_primitive_offset.fetch_add(num_merged);
                assert(id_offset < primitives_ids_capacity);
                assert(id_offset + num_merged <= primitives_ids_capacity);

                for (int i = 0; i < num_merged; i++) {
                    primitives_ids[id_offset + i] = cmds[idx_first + i].primitive_id;
                }

                // Create new cmd and fill in
                RenderCmd* cmd                                   = m_cmd_allocator->allocate();
                *cmd                                             = *cmds[idx_first].cmd;
                cmd->vert_buffers.buffers[cmd->primitive_buffer] = primitives_ids_buffer;
                cmd->vert_buffers.offsets[cmd->primitive_buffer] = int(id_offset * sizeof(int));
                cmd->call_params.instances                       = num_merged;

                SortableRenderCmd new_cmd;
                new_cmd.cmd = cmd;

                cmds[idx_write] = new_cmd;
            } else {
                // Allocate one id and setup
                const std::uint32_t id_offset = m_primitive_offset.fetch_add(1);
                assert(id_offset < primitives_ids_capacity);
                assert(id_offset + 1 <= primitives_ids_capacity);

                cmds[idx_write] = cmds[idx_read];

                // Copy existing cmd and slightly modify
                RenderCmd* cmd                                   = cmds[idx_write].cmd;
                cmd->vert_buffers.buffers[cmd->primitive_buffer] = primitives_ids_buffer;
                cmd->vert_buffers.offsets[cmd->primitive_buffer] = int(id_offset * sizeof(int));
            }

            idx_read += 1;
            idx_write += 1;
        }

        cmds.resize(idx_write);

        return StatusCode::Ok;
    }
    void MeshRenderCmdMerger::set_scene(RenderScene* scene) {
        m_scene = scene;
    }
    void MeshRenderCmdMerger::set_cmd_allocator(RenderCmdAllocator& allocator) {
        m_cmd_allocator = &allocator;
    }
    void MeshRenderCmdMerger::clear() {
        m_primitive_offset.store(0);
    }

}// namespace wmoge