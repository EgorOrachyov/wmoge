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

#include "core/log.hpp"
#include "core/status.hpp"
#include "core/synchronization.hpp"

#include <any>
#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace wmoge {

    /**
     * @brief Life-time type of ioc entry
    */
    enum class IocEntryType {
        Singleton = 0
    };

    /**
     * @class IocEntry
     * @brief Stores information about managed entry in IoC container
    */
    struct IocEntry {
        std::any                         instance;
        std::function<Status(std::any&)> factory;
        std::optional<std::type_index>   provided_type;
        std::optional<std::type_index>   source_type;
        IocEntryType                     type = IocEntryType::Singleton;
    };

    /**
     * @class IocContainer
     * @brief Inversion of control container for simpler dependency injection of global engine managers
     * 
     * IoC container allows to store and access singletons and factories for any kind of objects.
    */
    class IocContainer {
    public:
        IocContainer()                        = default;
        IocContainer(const IocContainer&)     = delete;
        IocContainer(IocContainer&&) noexcept = delete;

        void                     clear();
        void                     add(IocEntry&& entry);
        void                     erase(std::type_index entry_type);
        std::optional<IocEntry*> get(std::type_index entry_type);

        template<typename S, typename T, typename Factory>
        void bind_f(Factory&& factory);

        template<typename S, typename T>
        void bind_i(std::shared_ptr<T> instance);

        template<typename S>
        void bind();

        template<typename S>
        void unbind();

        template<typename S>
        std::optional<S*> resolve();

        template<typename S>
        S* resolve_v();

        template<typename S>
        static S* iresolve_v();

        static IocContainer* instance();

    private:
        std::unordered_map<std::type_index, IocEntry> m_entries;
        std::recursive_mutex                          m_mutex;
    };

    template<typename S, typename T, typename Factory>
    inline void IocContainer::bind_f(Factory&& factory) {
        std::lock_guard guard(m_mutex);

        IocEntry entry;
        entry.source_type   = typeid(S);
        entry.provided_type = typeid(T);
        entry.factory       = [f = std::move(factory)](std::any& out) -> Status {
            out = std::move(f());
            return StatusCode::Ok;
        };

        add(std::move(entry));
    }

    template<typename S, typename T>
    inline void IocContainer::bind_i(std::shared_ptr<T> instance) {
        std::lock_guard guard(m_mutex);

        bind_f<S, T>([i = std::move(instance)]() {
            return i;
        });
    }

    template<typename S>
    inline void IocContainer::bind() {
        std::lock_guard guard(m_mutex);

        bind_f<S, S>([]() {
            return std::make_shared<S>();
        });
    }

    template<typename S>
    inline void IocContainer::unbind() {
        std::lock_guard guard(m_mutex);

        erase(typeid(S));
    }

    template<typename S>
    inline std::optional<S*> IocContainer::resolve() {
        std::lock_guard guard(m_mutex);

        auto opt_entry = get(typeid(S));

        if (!opt_entry) {
            WG_LOG_ERROR("no such entry to resolve with type " << std::type_index(typeid(S)).name());
            return std::nullopt;
        }

        IocEntry& entry = *(opt_entry.value());

        if (entry.type != IocEntryType::Singleton) {
            WG_LOG_ERROR("non-singleton entry not supported for type " << std::type_index(typeid(S)).name());
            return std::nullopt;
        }

        if (!entry.instance.has_value()) {
            auto status = entry.factory(entry.instance);
            if (!status) {
                WG_LOG_ERROR("failed to instantiate object of type " << entry.provided_type.value().name());
                return std::nullopt;
            }
        }

        S* ptr = nullptr;

        try {
            ptr = std::any_cast<std::shared_ptr<S>>(entry.instance).get();
        } catch (const std::bad_any_cast&) {
            WG_LOG_ERROR("failed to cast entry to type " << std::type_index(typeid(S)).name()
                                                         << " from type " << entry.provided_type.value().name());
            assert(false);
        }

        if (!ptr) {
            WG_LOG_ERROR("instance is null " << std::type_index(typeid(S)).name());
            return std::nullopt;
        }

        return ptr;
    }

    template<typename S>
    inline S* IocContainer::resolve_v() {
        return resolve<S>().value();
    }

    template<typename S>
    inline S* IocContainer::iresolve_v() {
        return instance()->resolve_v<S>();
    }

}// namespace wmoge