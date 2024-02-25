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

#include "mesh_pass.hpp"

#include "system/engine.hpp"

#include <cassert>

namespace wmoge {

    bool MeshPassList::has_pass(MeshPassType pass_type) const {
        return m_mask.get(pass_type);
    }

    std::optional<MeshPass*> MeshPassList::get_pass(MeshPassType pass_type) {
        if (m_mask.get(pass_type)) {
            for (std::size_t i = 0; i < get_size(); i++) {
                if (m_passes[i].pass_type == pass_type) {
                    return &m_passes[i];
                }
            }

            assert(false && "Invalid invariant of mask state and list state");
        }

        return std::nullopt;
    }

    void MeshPassList::add_pass(MeshPass&& pass, bool overwrite) {
        if (!m_mask.get(pass.pass_type)) {
            m_passes.push_back(std::move(pass));
            m_mask.set(m_passes.back().pass_type);
            return;
        }

        if (overwrite) {
            for (std::size_t i = 0; i < get_size(); i++) {
                if (m_passes[i].pass_type == pass.pass_type) {
                    m_passes[i] = std::move(pass);
                    return;
                }
            }
        }
    }

    MeshPassProcessor::MeshPassProcessor() {
        Engine* engine   = Engine::instance();
        m_shader_manager = engine->shader_manager();
        m_gfx_driver     = engine->gfx_driver();
    }

}// namespace wmoge