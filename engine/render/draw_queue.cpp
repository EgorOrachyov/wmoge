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

#include "draw_queue.hpp"

#include "debug/profiler.hpp"
#include "render/draw_cmd.hpp"

#include <algorithm>

namespace wmoge {

    DrawCmdSortingKey DrawCmdSortingKey::make_overlay(RenderMaterial* material, int layer_id) {
        assert(layer_id >= 0);

        DrawCmdSortingKey key;

        // layers stack, higher value => draw on top, thus need to revers to sort using `<` predicate
        const std::uint32_t max_layer    = 0xffffffff;
        const std::uint64_t layer_key    = max_layer - std::uint32_t(layer_id);
        const std::uint64_t material_key = material->get_hash();

        // sort in layers first, and then in the same layer with the same material
        key.value = (layer_key << 32u) | (material_key << 0u);

        return key;
    }

    void DrawCmdQueue::push(DrawCmdSortingKey key, DrawCmd* cmd) {
        std::lock_guard guard(m_mutex);
        m_cmds.emplace_back(key, cmd);
    }
    void DrawCmdQueue::reserve(std::size_t size) {
        WG_AUTO_PROFILE_RENDER("DrawCmdQueue::reserve");

        std::lock_guard guard(m_mutex);
        m_cmds.reserve(size);
    }
    void DrawCmdQueue::clear() {
        WG_AUTO_PROFILE_RENDER("DrawCmdQueue::clear");

        std::lock_guard guard(m_mutex);
        m_cmds.clear();
    }

    void DrawCmdQueue::sort() {
        WG_AUTO_PROFILE_RENDER("DrawCmdQueue::sort");

        std::lock_guard guard(m_mutex);

        std::sort(m_cmds.begin(), m_cmds.end(), [](const auto& ent1, const auto& ent2) {
            return ent1.first.value < ent2.first.value;
        });
    }

    void DrawCmdQueue::execute(GfxDriver* driver, DrawUniformBuffer* pass_buffers, int pass_buffers_count) {
        WG_AUTO_PROFILE_RENDER("DrawCmdQueue::execute");

        std::lock_guard guard(m_mutex);

        bool                  pass_buffers_bound = false;
        const GfxPipeline*    prev_pipeline      = nullptr;
        const RenderMaterial* prev_material      = nullptr;

        for (auto entry : m_cmds) {
            const DrawCmd*              cmd         = entry.second;
            const DrawParams&           draw_params = cmd->draw_params;
            const DrawVertexBuffers&    vertices    = cmd->vertices;
            const DrawIndexBuffer&      indices     = cmd->indices;
            const DrawUniformBuffer&    constants   = cmd->constants;
            const DrawMaterialBindings& bindings    = cmd->bindings;
            const RenderMaterial*       material    = cmd->material;
            const GfxPipeline*          pipeline    = cmd->pipeline;

            // bind pipeline (check equality inside, but also do here for sure)
            if (prev_pipeline != pipeline) {
                if (!driver->bind_pipeline(Ref<GfxPipeline>(cmd->pipeline))) {
                    // if failed to bind pipeline here we cannot do draw due to
                    // - pipeline may be not compiled yet, waiting for shader, etc.
                    // - here may be some errors in construction, so must skip
                    continue;
                }
            }

            // bind vertex streams (from 0 to MAX_VERTEX_BUFFERS)
            for (int i = 0; i < DrawVertexBuffers::MAX_VERTEX_BUFFERS && vertices.buffers[i]; i++) {
                driver->bind_vert_buffer(Ref<GfxVertBuffer>(vertices.buffers[i]), i, vertices.offsets[i]);
            }

            // optional indices stream
            if (indices.buffer) {
                driver->bind_index_buffer(Ref<GfxIndexBuffer>(indices.buffer), indices.index_type, indices.offset);
            }

            // if first time here, bind only once per pass data
            if (!pass_buffers_bound) {
                for (int i = 0; i < pass_buffers_count; i++) {
                    DrawUniformBuffer& buffer   = pass_buffers[i];
                    GfxLocation        location = {DrawPassConsts::DRAW_SET_PER_PASS, buffer.location};
                    driver->bind_uniform_buffer(location, buffer.offset, buffer.range, Ref<GfxUniformBuffer>(buffer.buffer));
                }

                pass_buffers_bound = true;
            }

            // bind material
            if (prev_material != material) {
                const auto& textures   = material->get_textures();
                const auto& samples    = material->get_samplers();
                const auto& parameters = material->get_parameters();

                int textures_count   = int(textures.size());
                int parameters_range = int(parameters->size());

                for (int i = 0; i < textures_count; i++) {
                    GfxLocation location{DrawPassConsts::DRAW_SET_PER_MATERIAL, bindings.first_texture + i};
                    int         array_element = 0;
                    driver->bind_texture(location, array_element, textures[i], samples[i]);
                }

                GfxLocation location{DrawPassConsts::DRAW_SET_PER_MATERIAL, bindings.first_buffer};
                driver->bind_uniform_buffer(location, 0, parameters_range, parameters);
            }

            // bind draw constants
            if (constants.buffer) {
                GfxLocation location = {DrawPassConsts::DRAW_SET_PER_DRAW, constants.location};
                driver->bind_uniform_buffer(location, constants.offset, constants.range, Ref<GfxUniformBuffer>(constants.buffer));
            }

            // do draw call
            if (indices.buffer) {
                assert(draw_params.index_count >= 1);
                assert(draw_params.base_vertex >= 0);
                assert(draw_params.instance_count >= 1);

                driver->draw_indexed(draw_params.index_count, draw_params.base_vertex, draw_params.instance_count);
            } else {
                assert(draw_params.vertex_count >= 1);
                assert(draw_params.base_vertex >= 0);
                assert(draw_params.instance_count >= 1);

                driver->draw(draw_params.vertex_count, draw_params.base_vertex, draw_params.instance_count);
            }

            prev_pipeline = pipeline;
            prev_material = material;
        }
    }

    std::vector<std::pair<DrawCmdSortingKey, DrawCmd*>>& DrawCmdQueue::get_cmds() {
        return m_cmds;
    }
    const std::vector<std::pair<DrawCmdSortingKey, DrawCmd*>>& DrawCmdQueue::get_cmds() const {
        return m_cmds;
    }

}// namespace wmoge
