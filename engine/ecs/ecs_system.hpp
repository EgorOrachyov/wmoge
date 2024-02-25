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

#include "debug/profiler.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_memory.hpp"

#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

namespace wmoge {

    /**
     * @brief Type of a system
     */
    enum class EcsSystemType {
        Default
    };

    /**
     * @brief How system must be executed
     */
    enum class EcsSystemExecMode {
        OnMain,
        OnWorkers
    };

    /**
     * @brief Static function which can be used to implement a system interface
     */
    template<typename T, typename... TArgs>
    using EcsSystemNativeFunc = void (T::*)(class EcsWorld& world, const EcsEntity& entity, TArgs... args);

    /**
     * @class EcsSystem
     * @brief Base class for any system, which can be register within a world
     */
    class EcsSystem {
    public:
        virtual ~EcsSystem() = default;

        /**
         * @brief Called on system to process a batch of entities having the same archetype
         *
         * @note Use an optional `WG_ECS_SYSTEM_BIND` helper macro to implement this method.
         *
         * @param world World of entities
         * @param storage Archetype storage
         * @param start_entity Start entity to process in batch
         * @param count Total entities count within batch
         */
        virtual void process_batch(class EcsWorld& world, EcsArchStorage& storage, int start_entity, int count) = 0;

        [[nodiscard]] virtual EcsSystemType     get_type() const { return EcsSystemType::Default; }
        [[nodiscard]] virtual EcsSystemExecMode get_exec_mode() const { return EcsSystemExecMode::OnMain; }
        [[nodiscard]] virtual StringId          get_name() const  = 0;
        [[nodiscard]] virtual EcsQuery          get_query() const = 0;
    };

    /**
     * @brief Holds system information for execution within a world
     */
    struct EcsSystemInfo {
        EcsQuery                   query;        // system query, which archetypes its affects
        EcsSystemExecMode          exec_mode;    // execution mode
        std::shared_ptr<EcsSystem> system;       // cached system ptr
        std::vector<int>           filtered_arch;// pre-filtered arch idx to execute using this system
    };

#define WG_ECS_SYSTEM_BIND(System) EcsSystemBindHelper(this, &System::process).process_batch(world, storage, start_entity, count)

    /**
     * @class EcsSystemBindHelper
     * @brief Executor implementation for binding C++ systems
     *
     * @tparam T System type
     * @tparam TArgs Type of system args for processing
     */
    template<typename T, typename... TArgs>
    class EcsSystemBindHelper {
    public:
        static_assert(sizeof...(TArgs) <= 5, "supported auto binding is limited by num of components");

        EcsSystemBindHelper(T* system_ptr, EcsSystemNativeFunc<T, TArgs...> function_ptr);

        void process_batch(class EcsWorld& world, EcsArchStorage& storage, int start_entity, int count);

    private:
        T*                               m_system_ptr   = nullptr;
        EcsSystemNativeFunc<T, TArgs...> m_function_ptr = nullptr;
    };

    template<typename T, typename... TArgs>
    EcsSystemBindHelper<T, TArgs...>::EcsSystemBindHelper(T* system_ptr, EcsSystemNativeFunc<T, TArgs...> function_ptr) {
        m_system_ptr   = system_ptr;
        m_function_ptr = function_ptr;
    }

    template<typename T, typename... TArgs>
    void EcsSystemBindHelper<T, TArgs...>::process_batch(class EcsWorld& world, EcsArchStorage& storage, int start_entity, int count) {
        WG_AUTO_PROFILE_ECS_DECS("EcsSystemBindHelper::process_batch", m_system_ptr->get_name().str());

        static const auto num_args = sizeof...(TArgs);

        for (int i = 0; i < count; i++) {
            const int       entity_idx = start_entity + i;
            const EcsEntity entity     = storage.get_entity(entity_idx);

            if (entity.is_invalid()) {
                continue;
            }

#define WG_ECS_ACCESS_COMPONENT(at)                                                                                                 \
    using TypeComponent##at    = typename std::remove_reference<typename std::tuple_element<at, std::tuple<TArgs...>>::type>::type; \
    TypeComponent##at* ptr##at = storage.template get_component<TypeComponent##at>(entity_idx);

            if constexpr (num_args == 0) {
                (*m_system_ptr.*m_function_ptr)(world, entity);
            } else if constexpr (num_args == 1) {
                WG_ECS_ACCESS_COMPONENT(0);
                (*m_system_ptr.*m_function_ptr)(world, entity, *ptr0);
            } else if constexpr (num_args == 2) {
                WG_ECS_ACCESS_COMPONENT(0);
                WG_ECS_ACCESS_COMPONENT(1);
                (*m_system_ptr.*m_function_ptr)(world, entity, *ptr0, *ptr1);
            } else if constexpr (num_args == 3) {
                WG_ECS_ACCESS_COMPONENT(0);
                WG_ECS_ACCESS_COMPONENT(1);
                WG_ECS_ACCESS_COMPONENT(2);
                (*m_system_ptr.*m_function_ptr)(world, entity, *ptr0, *ptr1, *ptr2);
            } else if constexpr (num_args == 4) {
                WG_ECS_ACCESS_COMPONENT(0);
                WG_ECS_ACCESS_COMPONENT(1);
                WG_ECS_ACCESS_COMPONENT(2);
                WG_ECS_ACCESS_COMPONENT(3);
                (*m_system_ptr.*m_function_ptr)(world, entity, *ptr0, *ptr1, *ptr2, *ptr3);
            } else if constexpr (num_args == 5) {
                WG_ECS_ACCESS_COMPONENT(0);
                WG_ECS_ACCESS_COMPONENT(1);
                WG_ECS_ACCESS_COMPONENT(2);
                WG_ECS_ACCESS_COMPONENT(3);
                WG_ECS_ACCESS_COMPONENT(4);
                (*m_system_ptr.*m_function_ptr)(world, entity, *ptr0, *ptr1, *ptr2, *ptr3, *ptr4);
            }

#undef WG_ECS_ACCESS_COMPONENT
        }
    }

}// namespace wmoge