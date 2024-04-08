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

#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "io/archive.hpp"
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "math/mat.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"
#include "rtti/class.hpp"
#include "rtti/enum.hpp"
#include "rtti/struct.hpp"
#include "rtti/type.hpp"
#include "rtti/type_storage.hpp"

#include <cinttypes>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wmoge {

    template<typename T, typename Enable = void>
    struct RttiTypeOf;

    template<typename T>
    inline RttiType* rtti_type() {
        static RttiType* g_type = RttiTypeStorage::instance()->find_type(SID(RttiTypeOf<T>::name())).value_or(nullptr);

        if (!g_type) {
            Ref<RttiType> type = RttiTypeOf<T>::make();
            RttiTypeStorage::instance()->add(type);
            g_type = type.get();
        }

        return g_type;
    }

    template<typename T, typename Base>
    class RttiTypeT : public Base {
    public:
        RttiTypeT(Strid name) : Base(name, sizeof(T)) {
        }
        ~RttiTypeT() override = default;
        Status consturct(void* dst) const override {
            new (dst) T();
            return StatusCode::Ok;
        }
        Status copy(void* dst, const void* src) const override {
            *((T*) dst) = *((const T*) src);
            return StatusCode::Ok;
        }
        Status destruct(void* dst) const override {
            ((T*) dst)->~T();
            return StatusCode::Ok;
        }
        Status read_from_yaml(void* dst, YamlConstNodeRef node) const override {
            return yaml_read(node, *((T*) dst));
        }
        Status write_to_yaml(const void* src, YamlNodeRef node) const override {
            return yaml_write(node, *((const T*) src));
        }
        Status read_from_archive(void* dst, Archive& archive) const override {
            return archive_read(archive, *((T*) dst));
        }
        Status write_to_archive(const void* src, Archive& archive) const override {
            return archive_write(archive, *((T*) src));
        }
    };

    template<typename T>
    class RttiTypeEnumT : public RttiTypeT<T, RttiEnum> {
    public:
        using RttiEnum::m_num_values;
        using RttiEnum::m_strs;
        using RttiEnum::m_values;

        RttiTypeEnumT(Strid name) : RttiTypeT<T, RttiEnum>(name) {
            m_num_values = int(magic_enum::enum_count<T>());
            for (int i = 0; i < m_num_values; i++) {
                const T enum_value = magic_enum::enum_value<T>(i);
                m_values.push_back((int) enum_value);
                m_strs.push_back(SID(std::string(magic_enum::enum_name(enum_value))));
            }
        }
        ~RttiTypeEnumT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            s << magic_enum::enum_name(*((const T*) src));
            return StatusCode::Ok;
        }
    };

    template<typename T>
    class RttiTypeFundamentalT : public RttiTypeT<T, RttiType> {
    public:
        RttiTypeFundamentalT(Strid name) : RttiTypeT<T, RttiType>(name) {
        }
        ~RttiTypeFundamentalT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            s << *((const T*) src);
            return StatusCode::Ok;
        }
    };

    template<typename VecT, typename ElemT>
    class RttiTypeBaseVectorT : public RttiTypeT<VecT, RttiType> {
    public:
        RttiTypeBaseVectorT(Strid name) : RttiTypeT<VecT, RttiType>(name) {
            m_elem_type = rtti_type<ElemT>();
        }
        ~RttiTypeBaseVectorT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const VecT& vec = *((const VecT*) src);
            s << "[";
            for (const auto& elem : vec) {
                m_elem_type->to_string(&elem, s);
                s << ", ";
            }
            s << "]";
            return StatusCode::Ok;
        }

    private:
        RttiType* m_elem_type;
    };

    template<typename MapT, typename KeyT, typename ValueT>
    class RttiTypeBaseMapT : public RttiTypeT<MapT, RttiType> {
    public:
        RttiTypeBaseMapT(Strid name) : RttiTypeT<MapT, RttiType>(name) {
            m_key_type   = rtti_type<KeyT>();
            m_value_type = rtti_type<ValueT>();
        }
        ~RttiTypeBaseMapT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const MapT& map = *((const MapT*) src);
            s << "{";
            for (const auto& elem : map) {
                m_key_type->to_string(&(elem->first), s);
                s << ": ";
                m_value_type->to_string(&(elem->second), s);
                s << ", ";
            }
            s << "}";
            return StatusCode::Ok;
        }

    private:
        RttiType* m_key_type;
        RttiType* m_value_type;
    };

    template<typename SetT, typename KeyT>
    class RttiTypeBaseSetT : public RttiTypeT<SetT, RttiType> {
    public:
        RttiTypeBaseSetT(Strid name) : RttiTypeT<SetT, RttiType>(name) {
            m_key_type = rtti_type<KeyT>();
        }
        ~RttiTypeBaseSetT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const SetT& set = *((const SetT*) src);
            s << "{";
            for (const auto& elem : set) {
                m_key_type->to_string(&elem, s);
                s << ", ";
            }
            s << "}";
            return StatusCode::Ok;
        }

    private:
        RttiType* m_key_type;
    };

    template<typename E>
    struct RttiTypeOf<E, typename std::enable_if<std::is_enum<E>::value>::type> {
        static Strid name() {
            return SID(std::string(magic_enum::enum_type_name<E>()));
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeEnumT<E>>(name());
        }
    };

    template<>
    struct RttiTypeOf<std::int16_t> {
        static Strid name() {
            return SID("int16_t");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<std::int16_t>>(name());
        }
    };

    template<>
    struct RttiTypeOf<int> {
        static Strid name() {
            return SID("int");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<int>>(name());
        }
    };

    template<>
    struct RttiTypeOf<unsigned int> {
        static Strid name() {
            return SID("uint");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<unsigned int>>(name());
        }
    };

    template<>
    struct RttiTypeOf<float> {
        static Strid name() {
            return SID("float");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<float>>(name());
        }
    };

    template<>
    struct RttiTypeOf<bool> {
        static Strid name() {
            return SID("bool");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<bool>>(name());
        }
    };

    template<>
    struct RttiTypeOf<std::string> {
        static Strid name() {
            return SID("string");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<std::string>>(name());
        }
    };

    template<>
    struct RttiTypeOf<Strid> {
        static Strid name() {
            return SID("Strid");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<Strid>>(name());
        }
    };

    template<typename T, int N>
    struct RttiTypeOf<TVecN<T, N>> {
        using Vec = TVecN<T, N>;

        static Strid name() {
            return SID(std::string("Vec<") + rtti_type<T>()->get_str() + "," + std::to_string(N) + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<Vec>>(name());
        }
    };

    template<typename T>
    struct RttiTypeOf<TQuat<T>> {
        using Quat = TQuat<T>;

        static Strid name() {
            return SID(std::string("Quat<") + rtti_type<T>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeFundamentalT<Quat>>(name());
        }
    };

    template<typename E>
    struct RttiTypeOf<std::vector<E>> {
        using VecT = std::vector<E>;

        static Strid name() {
            return SID(std::string("vector<") + rtti_type<E>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBaseVectorT<VecT, E>>(name());
        }
    };

    template<typename E, std::size_t S>
    struct RttiTypeOf<ankerl::svector<E, S>> {
        using VecT = ankerl::svector<E, S>;

        static Strid name() {
            return SID(std::string("buffered_vector<") + rtti_type<E>()->get_str() + "," + std::to_string(S) + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBaseVectorT<VecT, E>>(name());
        }
    };

    template<typename K, typename V>
    struct RttiTypeOf<std::unordered_map<K, V>> {
        using MapT = std::unordered_map<K, V>;

        static Strid name() {
            return SID(std::string("unordered_map<") + rtti_type<K>()->get_str() + "," + rtti_type<V>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBaseMapT<MapT, K, V>>(name());
        }
    };

    template<typename K, typename V>
    struct RttiTypeOf<robin_hood::unordered_flat_map<K, V>> {
        using MapT = robin_hood::unordered_flat_map<K, V>;

        static Strid name() {
            return SID(std::string("flat_map<") + rtti_type<K>()->get_str() + "," + rtti_type<V>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBaseMapT<MapT, K, V>>(name());
        }
    };

}// namespace wmoge