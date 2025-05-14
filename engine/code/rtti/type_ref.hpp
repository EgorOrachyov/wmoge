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

#include "io/enum.hpp"
#include "io/stream.hpp"
#include "io/tree.hpp"
#include "rtti/traits.hpp"
#include "rtti/type.hpp"
#include "rtti/type_storage.hpp"

#include <cassert>
#include <type_traits>

namespace wmoge {

    template<typename T>
    class RttiRef {
    public:
        static_assert(std::is_base_of_v<RttiType, T>, "Must be an rtti type");

        RttiRef() = default;
        RttiRef(T* ref) : m_ref(ref) {}

        [[nodiscard]] bool operator==(const RttiRef& other) const { return m_ref == other.m_ref; }
        [[nodiscard]] bool operator!=(const RttiRef& other) const { return m_ref != other.m_ref; }

        operator bool() const { return m_ref != nullptr; }

        [[nodiscard]] T* operator->() const { return m_ref; }

        [[nodiscard]] bool is_empty() const { return !m_ref; }
        [[nodiscard]] bool is_not_empty() const { return m_ref; }
        [[nodiscard]] T*   get() const { return m_ref; }

    protected:
        T* m_ref = nullptr;
    };

    template<typename T>
    inline std::ostream& operator<<(std::ostream& stream, const RttiRef<T>& ref) {
        if (ref) {
            stream << '\'' << ref->get_name() << '\'';
        } else {
            stream << "null";
        }
        return stream;
    }

    template<typename T>
    Status tree_read(IoContext& context, IoTree& tree, RttiRef<T>& ref) {
        Strid id;
        WG_TREE_READ(context, tree, id);
        if (!id.empty()) {
            std::optional<RttiType*> type_opt = context.get<RttiTypeStorage*>()->find_type(id);
            if (!type_opt) {
                return StatusCode::NoType;
            }
            T* type_ptr = dynamic_cast<T*>(*type_opt);
            if (!type_ptr) {
                return StatusCode::InvalidState;
            }
            ref = RttiRef<T>(type_ptr);
        }
        return WG_OK;
    }

    template<typename T>
    Status tree_write(IoContext& context, IoTree& tree, const RttiRef<T>& ref) {
        Strid id = ref ? ref->get_name() : Strid();
        WG_TREE_WRITE(context, tree, id);
        return WG_OK;
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, RttiRef<T>& ref) {
        Strid id;
        WG_ARCHIVE_READ(context, stream, id);
        if (!id.empty()) {
            std::optional<RttiType*> type_opt = context.get<RttiTypeStorage*>()->find_type(id);
            if (!type_opt) {
                return StatusCode::NoType;
            }
            T* type_ptr = dynamic_cast<T*>(*type_opt);
            if (!type_ptr) {
                return StatusCode::InvalidState;
            }
            ref = RttiRef<T>(type_ptr);
        }
        return WG_OK;
    }

    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const RttiRef<T>& ref) {
        Strid id = ref ? ref->get_name() : Strid();
        WG_ARCHIVE_WRITE(context, stream, id);
        return WG_OK;
    }

    template<typename T>
    struct RttiTypeOf<RttiRef<T>, typename std::enable_if<std::is_base_of_v<RttiType, T>>::type> {
        static Strid name() {
            const std::string str_archetype = Enum::to_str(T::static_archetype());
            return SID(std::string("rtti<") + str_archetype + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<RttiRef<T>>>(name());
        }
    };

    using RttiRefType   = RttiRef<RttiType>;
    using RttiRefStruct = RttiRef<RttiStruct>;
    using RttiRefClass  = RttiRef<RttiClass>;

    template<typename T>
    class RttiSubclass : public RttiRefClass {
    public:
        static_assert(std::is_base_of_v<RttiObject, T>, "Must be an rtti object type");

        RttiSubclass()                        = default;
        RttiSubclass(const RttiSubclass&)     = default;
        RttiSubclass(RttiSubclass&&) noexcept = default;

        RttiSubclass(RttiClass* rtti) {
            if (rtti) {
                assert(rtti->is_subtype_of(T::get_class_static()));
                m_ref = rtti;
            }
        }

        RttiSubclass(RttiRefClass rtti) : RttiSubclass(rtti.get()) {
        }

        template<typename G>
        operator RttiSubclass<G>() const {
            static_assert(std::is_base_of_v<G, T>);
            return RttiSubclass<G>(m_ref);
        }

        template<typename G>
        bool is_subtype_of() const {
            return m_ref && m_ref->is_subtype_of(G::get_class_static());
        }

        template<typename G>
        RttiSubclass<G> cast() const {
            return is_subtype_of<G>() ? RttiSubclass<G>(m_ref) : RttiSubclass<G>();
        }
    };

    template<typename T>
    Status tree_read(IoContext& context, IoTree& tree, RttiSubclass<T>& ref) {
        RttiRefClass class_ref;
        WG_CHECKED(tree_read(context, tree, class_ref));
        if (class_ref && !class_ref->is_subtype_of(T::get_class_static())) {
            return StatusCode::InvalidData;
        }
        ref = RttiSubclass<T>(class_ref);
        return WG_OK;
    }

    template<typename T>
    Status tree_write(IoContext& context, IoTree& tree, const RttiSubclass<T>& ref) {
        WG_CHECKED(tree_write(context, tree, (const RttiRefClass&) ref));
        return WG_OK;
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, RttiSubclass<T>& ref) {
        RttiRefClass class_ref;
        WG_CHECKED(stream_read(context, stream, class_ref));
        if (class_ref && !class_ref->is_subtype_of(T::get_class_static())) {
            return StatusCode::InvalidData;
        }
        ref = RttiSubclass<T>(class_ref);
        return WG_OK;
    }

    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const RttiSubclass<T>& ref) {
        WG_CHECKED(stream_write(context, stream, (const RttiRefClass&) ref));
        return WG_OK;
    }

    template<typename T>
    struct RttiTypeOf<RttiSubclass<T>, typename std::enable_if<std::is_base_of_v<RttiObject, T>>::type> {
        static Strid name() {
            return SID(std::string("rttisubclass<") + T::get_class_static()->get_name().str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<RttiSubclass<T>>>(name());
        }
    };

}// namespace wmoge