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
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_memory.hpp"

namespace wmoge {

    /** @brief Component access and modification type in query  */
    enum class EcsComponentAccess {
        ReadOnly = 0,
        ReadWrite
    };

    /** @brief Describes presence of components in query */
    enum class EcsComponentPresence {
        Required = 0,
        Optional,
        Exclude
    };

    /**
     * @class EcsAccess
     * @brief Configures query to iterate over specific type of entities
     */
    struct EcsAccess {
        using Bitset = std::bitset<EcsLimits::MAX_COMPONENTS>;

        Bitset referenced;
        Bitset read_only;
        Bitset read_write;
        Bitset requireds;
        Bitset optionals;
        Bitset excludes;
        Strid  name;

        EcsAccess() = default;

        template<typename Component>
        EcsAccess& add(EcsComponentPresence presence = EcsComponentPresence::Required, EcsComponentAccess access = EcsComponentAccess::ReadOnly) {
            assert(!referenced.test(Component::IDX));
            referenced.set(Component::IDX);

            if (presence == EcsComponentPresence::Exclude) {
                excludes.set(Component::IDX);
                return *this;
            }
            if (presence == EcsComponentPresence::Required) {
                requireds.set(Component::IDX);
            }
            if (presence == EcsComponentPresence::Optional) {
                optionals.set(Component::IDX);
            }
            if (access == EcsComponentAccess::ReadOnly) {
                read_only.set(Component::IDX);
            }
            if (access == EcsComponentAccess::ReadWrite) {
                read_write.set(Component::IDX);
            }
            return *this;
        }

        template<typename Component>
        EcsAccess& exclude() {
            return add<Component>(EcsComponentPresence::Exclude);
        }

        template<typename Component>
        EcsAccess& require(EcsComponentAccess access = EcsComponentAccess::ReadOnly) {
            return add<Component>(EcsComponentPresence::Required, access);
        }

        template<typename Component>
        EcsAccess& optional(EcsComponentAccess access = EcsComponentAccess::ReadOnly) {
            return add<Component>(EcsComponentPresence::Optional, access);
        }

        bool match(const EcsArch& arch) const {
            if ((requireds & arch) != requireds) {
                return false;
            }
            if ((excludes & arch).any()) {
                return false;
            }
            return true;
        }

        [[nodiscard]] std::string to_string() const { return ""; }
    };

    inline std::ostream& operator<<(std::ostream& stream, const EcsAccess& query) {
        stream << query.to_string();
        return stream;
    }

    /**
     * @class EcsQueryContext
     * @brief Context passed to execute function when query is executed
     */
    class EcsQueryContext {
    public:
        EcsQueryContext(class EcsWorld& world, EcsArchStorage& storage, EcsAccess query, int start, int count)
            : m_world(world),
              m_storage(storage),
              m_arch(storage.get_arch()),
              m_query(query),
              m_range_start(start),
              m_range_count(count) {
        }

        template<typename Functor>
        void for_each(Functor f);

        template<typename Component>
        [[nodiscard]] bool has_component() const;

        template<typename Component>
        [[nodiscard]] Component& get_component(int entity_idx);

        [[nodiscard]] EcsEntity get_entity(int entity_idx);

        [[nodiscard]] class EcsWorld&  get_world() const { return m_world; }
        [[nodiscard]] const EcsAccess& get_query() const { return m_query; }
        [[nodiscard]] int              get_start_idx() const { return m_range_start; }
        [[nodiscard]] int              get_count() const { return m_range_count; }

    private:
        class EcsWorld& m_world;
        EcsArchStorage& m_storage;
        EcsAccess       m_query;
        EcsArch         m_arch;
        int             m_range_start = -1;
        int             m_range_count = 0;
    };

    /** @brief Function used to execute ecs query */
    using EcsQueuryFunction = std::function<void(EcsQueryContext& context)>;

    template<typename Functor>
    inline void EcsQueryContext::for_each(Functor f) {
        for (int i = m_range_start; i < m_range_start + m_range_count; i++) {
            f(i);
        }
    }

    template<typename Component>
    inline bool EcsQueryContext::has_component() const {
        return m_arch.has_component<Component>();
    }

    template<typename Component>
    inline Component& EcsQueryContext::get_component(int entity_idx) {
        return *m_storage.get_component<Component>(entity_idx);
    }

    inline EcsEntity wmoge::EcsQueryContext::get_entity(int entity_idx) {
        return m_storage.get_entity(entity_idx);
    }

    template<typename AccessType>
    class EcsQuery : public EcsQueryContext {
    public:
        EcsQuery(EcsQueryContext& context)
            : EcsQueryContext(context) {

            assert(validate_access());
        }

        [[nodiscard]] bool validate_access() const {
            const EcsAccess& access = get_query();

            if ((access_type.requireds & access.requireds) != access.requireds) {
                return false;
            }

            return true;
        }

        const AccessType access_type;
    };

}// namespace wmoge