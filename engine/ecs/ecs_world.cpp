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

#include "ecs_world.hpp"

namespace wmoge {

    EcsEntity EcsWorld::make_entity(const EcsArch& arch) {
        assert(arch.any());

        if (m_storage_idx.find(arch) == m_storage_idx.end()) {
            const int arch_idx  = int(m_storage.size());
            m_storage_idx[arch] = arch_idx;
            m_storage.push_back(std::make_unique<EcsArchStorage>(arch, arch_idx));
        }

        assert(m_storage_idx.find(arch) != m_storage_idx.end());

        const int arch_idx = m_storage_idx[arch];
        return m_storage[arch_idx]->make_entity();
    }

    void EcsWorld::destroy_entity(const EcsEntity& entity) {
        assert(entity.is_valid());
        assert(entity.arch < m_storage.size());

        m_storage[entity.arch]->destroy_entity(entity);
    }

    bool EcsWorld::is_alive(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(entity.arch < m_storage.size());

        return m_storage[entity.arch]->is_alive(entity);
    }

    EcsArch EcsWorld::get_arch(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(entity.arch < m_storage.size());

        return m_storage[entity.arch]->get_arch();
    }

    void EcsWorld::sync() {
        m_queue.flush();
    }

}// namespace wmoge