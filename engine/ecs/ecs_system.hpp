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

#include <cassert>
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
        Update,// Runtime system for every frame updates
        Destroy// Called on entity deletion
    };

    /**
     * @brief How system must be executed
     */
    enum class EcsSystemExecMode {
        SingleThread,// One thread only without parallel speed up
        WorkerThreads// In task manager with multiple parallel tasks
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

        [[nodiscard]] virtual EcsSystemType     get_type() const { return EcsSystemType::Update; }
        [[nodiscard]] virtual EcsSystemExecMode get_exec_mode() const { return EcsSystemExecMode::SingleThread; }
        [[nodiscard]] virtual Strid             get_name() const  = 0;
        [[nodiscard]] virtual EcsQuery          get_query() const = 0;
    };

    /**
     * @brief Shader pointer to a ecs system
    */
    using EcsSystemPtr = std::shared_ptr<EcsSystem>;

    /**
     * @brief Holds system information for execution within a world
     */
    struct EcsSystemInfo {
        EcsQuery          query;        // system query, which archetypes its affects
        EcsSystemType     type;         // system type (exec, deletion, etc.)
        EcsSystemExecMode exec_mode;    // execution mode
        EcsSystemPtr      system;       // cached system ptr
        std::vector<int>  filtered_arch;// pre-filtered arch idx to execute using this system
    };

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
        static_assert(sizeof...(TArgs) <= 6, "supported auto binding is limited by num of components");

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

            assert(entity.is_valid());

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
            } else if constexpr (num_args == 6) {
                WG_ECS_ACCESS_COMPONENT(0);
                WG_ECS_ACCESS_COMPONENT(1);
                WG_ECS_ACCESS_COMPONENT(2);
                WG_ECS_ACCESS_COMPONENT(3);
                WG_ECS_ACCESS_COMPONENT(4);
                WG_ECS_ACCESS_COMPONENT(5);
                (*m_system_ptr.*m_function_ptr)(world, entity, *ptr0, *ptr1, *ptr2, *ptr3, *ptr4, *ptr5);
            }

#undef WG_ECS_ACCESS_COMPONENT
        }
    }

    /**
     * @class EcsSystemQueryHelper
     * @brief Utility to derive query type
     *
     * @tparam T System type
     * @tparam TArgs Type of system args for processing
     */
    template<typename T, typename... TArgs>
    class EcsSystemQueryHelper {
    public:
        static_assert(sizeof...(TArgs) <= 6, "supported auto binding is limited by num of components");

        EcsSystemQueryHelper(const T* system_ptr, EcsSystemNativeFunc<T, TArgs...> function_ptr);

        EcsQuery get_query() const;

    private:
        const T*                         m_system_ptr   = nullptr;
        EcsSystemNativeFunc<T, TArgs...> m_function_ptr = nullptr;
    };

    template<typename T, typename... TArgs>
    inline EcsSystemQueryHelper<T, TArgs...>::EcsSystemQueryHelper(const T* system_ptr, EcsSystemNativeFunc<T, TArgs...> function_ptr) {
        m_system_ptr   = system_ptr;
        m_function_ptr = function_ptr;
    }

    template<typename T, typename... TArgs>
    inline EcsQuery EcsSystemQueryHelper<T, TArgs...>::get_query() const {
        EcsQuery query;

        std::tuple<std::remove_reference_t<TArgs>...> pack;
        constexpr std::size_t                         N = std::tuple_size_v<decltype(pack)>;

#define WG_ECS_PROCESS_ARG(id)                                   \
    if constexpr (id < N) {                                      \
        using Arg     = std::tuple_element_t<0, decltype(pack)>; \
        using NoConst = std::remove_const_t<Arg>;                \
        if constexpr (std::is_same_v<Arg, NoConst>) {            \
            query.set_read<NoConst>();                           \
        } else {                                                 \
            query.set_write<NoConst>();                          \
        }                                                        \
    }

        WG_ECS_PROCESS_ARG(0)
        WG_ECS_PROCESS_ARG(1)
        WG_ECS_PROCESS_ARG(2)
        WG_ECS_PROCESS_ARG(3)
        WG_ECS_PROCESS_ARG(4)
        WG_ECS_PROCESS_ARG(5)

#undef WG_ECS_PROCESS_ARG

        return query;
    }

#define WG_ECS_SYSTEM_BIND(System) \
    EcsSystemBindHelper(this, &System::process).process_batch(world, storage, start_entity, count)

#define WG_ECS_SYSTEM(name, type, mode)                                                                        \
    ~name() = default;                                                                                         \
    void process_batch(class EcsWorld& world, EcsArchStorage& storage, int start_entity, int count) override { \
        WG_AUTO_PROFILE_ECS(#name "::process_batch");                                                          \
        EcsSystemBindHelper(this, &name::process).process_batch(world, storage, start_entity, count);          \
    }                                                                                                          \
    [[nodiscard]] EcsSystemType get_type() const override {                                                    \
        return EcsSystemType::type;                                                                            \
    }                                                                                                          \
    [[nodiscard]] EcsSystemExecMode get_exec_mode() const override {                                           \
        return EcsSystemExecMode::mode;                                                                        \
    }                                                                                                          \
    [[nodiscard]] Strid get_name() const override {                                                            \
        return SID(#name);                                                                                     \
    }                                                                                                          \
    [[nodiscard]] EcsQuery get_query() const override {                                                        \
        return EcsSystemQueryHelper(this, &name::process).get_query();                                         \
    }

}// namespace wmoge