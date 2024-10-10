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

#include "pfx_scene.hpp"

#include "profiler/profiler_cpu.hpp"

#include <cassert>

namespace wmoge {

    void PfxScene::add_emitter(const Ref<PfxEmitter>& emitter) {
        std::lock_guard guard(m_mutex);

        assert(m_emitters.find(emitter) == m_emitters.end());
        m_emitters.emplace(emitter);
    }
    void PfxScene::remove_emitter(const Ref<PfxEmitter>& emitter) {
        std::lock_guard guard(m_mutex);

        assert(m_emitters.find(emitter) != m_emitters.end());
        m_emitters.erase(emitter);
    }
    void PfxScene::update(float dt) {
        WG_PROFILE_CPU_PFX("PfxScene::update");

        for (auto& emitter : m_emitters) {
            emitter->advance_time(dt);
            emitter->update();
        }
    }

}// namespace wmoge