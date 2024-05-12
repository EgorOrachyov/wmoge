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

#include "ecs_registry.hpp"

#include "system/config.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    EcsRegistry::EcsRegistry() {
        Config* config = IocContainer::iresolve_v<Config>();

        config->get_int(SID("ecs.chunk_size"), m_chunk_size);
        config->get_int(SID("ecs.expand_size"), m_expand_size);

        m_entity_pool = std::make_unique<MemPool>(m_chunk_size * sizeof(EcsEntity), m_expand_size);
    }

    int EcsRegistry::get_component_idx(const Strid& name) {
        assert(m_components_name_to_idx.find(name) != m_components_name_to_idx.end());
        return m_components_name_to_idx[name];
    }

    const EcsComponentInfo& EcsRegistry::get_component_info(const Strid& name) {
        assert(m_components_name_to_idx.find(name) != m_components_name_to_idx.end());
        return m_components_info[m_components_name_to_idx[name]];
    }
    const EcsComponentInfo& EcsRegistry::get_component_info(int idx) {
        assert(idx < EcsLimits::MAX_COMPONENTS);
        return m_components_info[idx];
    }

    MemPool& EcsRegistry::get_component_pool(int idx) {
        assert(idx < EcsLimits::MAX_COMPONENTS);
        assert(m_components_pool[idx].get());
        return *(m_components_pool[idx]);
    }
    MemPool& EcsRegistry::get_entity_pool() {
        return *m_entity_pool;
    }

}// namespace wmoge