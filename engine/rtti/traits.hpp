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
#include "core/data.hpp"
#include "core/date_time.hpp"
#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "core/mask.hpp"
#include "core/sha256.hpp"
#include "core/status.hpp"
#include "core/uuid.hpp"
#include "io/archive.hpp"
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "math/mat.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"
#include "rtti/builtin.hpp"
#include "rtti/class.hpp"
#include "rtti/enum.hpp"
#include "rtti/object.hpp"
#include "rtti/struct.hpp"
#include "rtti/type.hpp"
#include "rtti/type_storage.hpp"

#include <cinttypes>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>
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
            g_type = RttiTypeStorage::instance()->find_type(SID(RttiTypeOf<T>::name())).value_or(nullptr);

            if (!g_type) {
                Ref<RttiType> type = RttiTypeOf<T>::make();
                RttiTypeStorage::instance()->add(type);
                g_type = type.get();
            }
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
            return WG_OK;
        }
        Status copy(void* dst, const void* src) const override {
            *((T*) dst) = *((const T*) src);
            return WG_OK;
        }
        Status destruct(void* dst) const override {
            ((T*) dst)->~T();
            return WG_OK;
        }
        Status read_from_yaml(void* dst, YamlConstNodeRef node, IoContext& context) const override {
            return yaml_read(context, node, *((T*) dst));
        }
        Status write_to_yaml(const void* src, YamlNodeRef node, IoContext& context) const override {
            return yaml_write(context, node, *((const T*) src));
        }
        Status read_from_archive(void* dst, Archive& archive, IoContext& context) const override {
            return archive_read(context, archive, *((T*) dst));
        }
        Status write_to_archive(const void* src, Archive& archive, IoContext& context) const override {
            return archive_write(context, archive, *((T*) src));
        }
    };

    template<typename PairT, typename KeyT, typename ValueT>
    class RttiTypePairT : public RttiTypeT<PairT, RttiTypePair> {
    public:
        RttiTypePairT(Strid name) : RttiTypeT<PairT, RttiTypePair>(name) {
            RttiTypePair::m_key_type   = rtti_type<KeyT>();
            RttiTypePair::m_value_type = rtti_type<ValueT>();
        }
        ~RttiTypePairT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const PairT& pair = *((const PairT*) src);
            s << "{";
            RttiTypePair::m_key_type->to_string(&(pair.first), s);
            s << ":";
            RttiTypePair::m_value_type->to_string(&(pair.first), s);
            s << "}";
            return WG_OK;
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
            return WG_OK;
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
            return WG_OK;
        }
    };

    template<typename VecT, typename ValueT, int N>
    class RttiTypeVecT : public RttiTypeT<VecT, RttiTypeVec> {
    public:
        RttiTypeVecT(Strid name) : RttiTypeT<VecT, RttiTypeVec>(name) {
            RttiTypeVec::m_value_type = rtti_type<ValueT>();
            RttiTypeVec::m_dimension  = N;
        }
        ~RttiTypeVecT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            s << *((const VecT*) src);
            return WG_OK;
        }
    };

    template<typename MaskT, typename ValueT, int N>
    class RttiTypeMaskT : public RttiTypeT<MaskT, RttiTypeMask> {
    public:
        RttiTypeMaskT(Strid name) : RttiTypeT<MaskT, RttiTypeMask>(name) {
            RttiTypeMask::m_value_type = rtti_type<ValueT>();
            RttiTypeMask::m_dimension  = N;
        }
        ~RttiTypeMaskT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            s << *((const MaskT*) src);
            return WG_OK;
        }
    };

    template<typename VecT, typename ElemT>
    class RttiTypeBaseVectorT : public RttiTypeT<VecT, RttiTypeVector> {
    public:
        RttiTypeBaseVectorT(Strid name) : RttiTypeT<VecT, RttiTypeVector>(name) {
            RttiTypeVector::m_value_type = rtti_type<ElemT>();
        }
        ~RttiTypeBaseVectorT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const VecT& vec = *((const VecT*) src);
            s << "[";
            for (const auto& elem : vec) {
                WG_CHECKED(RttiTypeVector::m_value_type->to_string(&elem, s));
                s << ", ";
            }
            s << "]";
            return WG_OK;
        }
        Status add_element(void* src) const override {
            VecT& vec = *((VecT*) src);
            vec.emplace_back();
            return WG_OK;
        }
        Status remove_element(void* src, int index) const override {
            VecT& vec = *((VecT*) src);
            if (index < vec.size()) {
                vec.erase(vec.begin() + index);
            }
            return WG_OK;
        }
        bool is_listable() const override {
            return true;
        }
    };

    template<typename MapT, typename KeyT, typename ValueT>
    class RttiTypeBaseMapT : public RttiTypeT<MapT, RttiTypeMap> {
    public:
        RttiTypeBaseMapT(Strid name) : RttiTypeT<MapT, RttiTypeMap>(name) {
            RttiTypeMap::m_key_type   = rtti_type<KeyT>();
            RttiTypeMap::m_value_type = rtti_type<ValueT>();
        }
        ~RttiTypeBaseMapT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const MapT& map = *((const MapT*) src);
            s << "{";
            for (const auto& elem : map) {
                WG_CHECKED(RttiTypeMap::m_key_type->to_string(&(elem.first), s));
                s << ": ";
                WG_CHECKED(RttiTypeMap::m_value_type->to_string(&(elem.second), s));
                s << ", ";
            }
            s << "}";
            return WG_OK;
        }
        Status add_element(void* src) const override {
            MapT& map = *((MapT*) src);
            map.emplace();
            return WG_OK;
        }
        Status remove_element(void* src, int index) const override {
            MapT& map = *((MapT*) src);
            if (index < map.size()) {
                map.erase(std::next(map.begin(), index));
            }
            return WG_OK;
        }
        bool is_listable() const override {
            return true;
        }
    };

    template<typename SetT, typename KeyT>
    class RttiTypeBaseSetT : public RttiTypeT<SetT, RttiTypeSet> {
    public:
        RttiTypeBaseSetT(Strid name) : RttiTypeT<SetT, RttiTypeSet>(name) {
            RttiTypeSet::m_value_type = rtti_type<KeyT>();
        }
        ~RttiTypeBaseSetT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const SetT& set = *((const SetT*) src);
            s << "{";
            for (const auto& elem : set) {
                WG_CHECKED(RttiTypeSet::m_value_type->to_string(&elem, s));
                s << ", ";
            }
            s << "}";
            return WG_OK;
        }
        Status add_element(void* src) const override {
            SetT& set = *((SetT*) src);
            set.emplace();
            return WG_OK;
        }
        Status remove_element(void* src, int index) const override {
            SetT& set = *((SetT*) src);
            if (index < set.size()) {
                set.erase(std::next(set.begin(), index));
            }
            return WG_OK;
        }
        bool is_listable() const override {
            return true;
        }
    };

    template<typename PtrT>
    class RttiTypeRefT : public RttiTypeT<Ref<PtrT>, RttiTypeRef> {
    public:
        using ParentT = RttiTypeT<Ref<PtrT>, RttiTypeRef>;
        using RefT    = Ref<PtrT>;

        RttiTypeRefT(Strid name) : ParentT(name) {
            RttiTypeRef::m_value_type = rtti_type<PtrT>();
        }
        ~RttiTypeRefT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const RefT& self = *((const RefT*) src);
            if (self) {
                return RttiTypeRef::m_value_type->to_string(self.get(), s);
            } else {
                s << "nil";
            }
            return WG_OK;
        }
    };

    template<typename T>
    class RttiTypeOptionalT : public RttiTypeT<std::optional<T>, RttiTypeOptional> {
    public:
        using ParentT   = RttiTypeT<std::optional<T>, RttiTypeOptional>;
        using OptionalT = std::optional<T>;

        RttiTypeOptionalT(Strid name) : ParentT(name) {
            RttiTypeOptional::m_value_type = rtti_type<T>();
        }
        ~RttiTypeOptionalT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            const OptionalT& self = *((const OptionalT*) src);
            if (self) {
                return RttiTypeOptional::m_value_type->to_string(&self.value(), s);
            } else {
                s << "nil";
            }
            return WG_OK;
        }
    };

    template<typename T>
    struct RttiTypeOf<Ref<T>, typename std::enable_if<std::is_base_of_v<RttiObject, T>>::type> {
        static Strid name() {
            return SID(std::string("ref<") + rtti_type<T>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeRefT<T>>(name());
        }
    };

    template<typename T>
    struct RttiTypeOf<std::optional<T>> {
        static Strid name() {
            return SID(std::string("optional<") + rtti_type<T>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeOptionalT<T>>(name());
        }
    };

    template<typename KeyT, typename ValueT>
    struct RttiTypeOf<std::pair<KeyT, ValueT>> {
        using PairT = std::pair<KeyT, ValueT>;

        static Strid name() {
            return SID(std::string("pair<") + rtti_type<KeyT>()->get_str() + "," + rtti_type<ValueT>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypePairT<PairT, KeyT, ValueT>>(name());
        }
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

#define WG_RTTI_FUNDAMENTAL_DECL(type, pretty_name)              \
    template<>                                                   \
    struct RttiTypeOf<type> {                                    \
        static Strid name() {                                    \
            return SID(pretty_name);                             \
        }                                                        \
        static Ref<RttiType> make() {                            \
            return make_ref<RttiTypeFundamentalT<type>>(name()); \
        }                                                        \
    }

#define WG_RTTI_DECL(type, pretty_name) WG_RTTI_FUNDAMENTAL_DECL(type, pretty_name)

    WG_RTTI_FUNDAMENTAL_DECL(std::int16_t, "int16_t");
    WG_RTTI_FUNDAMENTAL_DECL(int, "int");
    WG_RTTI_FUNDAMENTAL_DECL(unsigned int, "uint");
    WG_RTTI_FUNDAMENTAL_DECL(float, "float");
    WG_RTTI_FUNDAMENTAL_DECL(bool, "bool");
    WG_RTTI_FUNDAMENTAL_DECL(std::string, "string");
    WG_RTTI_FUNDAMENTAL_DECL(Strid, "strid");
    WG_RTTI_FUNDAMENTAL_DECL(UUID, "uuid");
    WG_RTTI_FUNDAMENTAL_DECL(Sha256, "sha256");
    WG_RTTI_FUNDAMENTAL_DECL(DateTime, "datetime");
    WG_RTTI_FUNDAMENTAL_DECL(Status, "status");
    WG_RTTI_FUNDAMENTAL_DECL(Ref<Data>, "data");

    template<typename T, int N>
    struct RttiTypeOf<Mask<T, N>> {
        using MaskType = Mask<T, N>;

        static Strid name() {
            return SID(std::string("mask<") + rtti_type<T>()->get_str() + "," + std::to_string(N) + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeMaskT<MaskType, T, N>>(name());
        }
    };

    template<typename T, int N>
    struct RttiTypeOf<TVecN<T, N>> {
        using Vec = TVecN<T, N>;

        static Strid name() {
            return SID(std::string("vec<") + rtti_type<T>()->get_str() + "," + std::to_string(N) + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeVecT<Vec, T, N>>(name());
        }
    };

    template<typename T>
    struct RttiTypeOf<TQuat<T>> {
        using Quat = TQuat<T>;

        static Strid name() {
            return SID(std::string("quat<") + rtti_type<T>()->get_str() + ">");
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
            return SID(std::string("bvector<") + rtti_type<E>()->get_str() + "," + std::to_string(S) + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBaseVectorT<VecT, E>>(name());
        }
    };

    template<typename K>
    struct RttiTypeOf<std::unordered_set<K>> {
        using SetT = std::unordered_set<K>;

        static Strid name() {
            return SID(std::string("uset<") + rtti_type<K>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBaseSetT<SetT, K>>(name());
        }
    };

    template<typename K, typename V>
    struct RttiTypeOf<std::unordered_map<K, V>> {
        using MapT = std::unordered_map<K, V>;

        static Strid name() {
            return SID(std::string("umap<") + rtti_type<K>()->get_str() + "," + rtti_type<V>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBaseMapT<MapT, K, V>>(name());
        }
    };

    template<typename K, typename V>
    struct RttiTypeOf<robin_hood::unordered_flat_map<K, V>> {
        using MapT = robin_hood::unordered_flat_map<K, V>;

        static Strid name() {
            return SID(std::string("fmap<") + rtti_type<K>()->get_str() + "," + rtti_type<V>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBaseMapT<MapT, K, V>>(name());
        }
    };

    template<typename StructT>
    class RttiStructT : public RttiStruct {
    public:
        RttiStructT() : RttiStruct(StructT::get_class_name_static(), sizeof(StructT), StructT::get_parent_class_static()) {}
        ~RttiStructT() override = default;
        Status consturct(void* dst) const override {
            new (dst) StructT();
            return WG_OK;
        }
        Status destruct(void* dst) const override {
            ((StructT*) dst)->~StructT();
            return WG_OK;
        }
        template<typename T>
        void add_field(Strid name, std::size_t offset_of, RttiMetaData meta_data) {
            RttiField field(name, rtti_type<T>(), sizeof(T), offset_of);
            field.set_metadata(std::move(meta_data));
            RttiStruct::add_field(std::move(field));
        }
    };

    template<typename ArgT>
    struct RttiArgInfo {
        using ActualType    = ArgT;
        using RemoveRef     = typename std::remove_reference_t<ActualType>;
        using RemovePointer = typename std::remove_pointer_t<RemoveRef>;
        using RemoveConst   = typename std::remove_const_t<RemovePointer>;
        using BaseType      = RemoveConst;

        static constexpr bool is_ref   = std::is_lvalue_reference<ActualType>::value;
        static constexpr bool is_ptr   = std::is_pointer<RemoveRef>::value;
        static constexpr bool is_const = std::is_const<RemovePointer>::value;
    };

    template<typename TClass, typename TSignature, typename TRet, typename... TArgs>
    class RttiFunctionT : public RttiFunction {
    public:
        RttiFunctionT(Strid name, TSignature p_method, std::size_t stack_size, std::vector<RttiParamInfo> args, RttiParamInfo ret) : RttiFunction(name, sizeof(p_method), stack_size, std::move(args), std::move(ret)) {
            m_p_method = p_method;
        }
        ~RttiFunctionT() override = default;

        Status call(RttiFrame& frame, class RttiObject* p_object, void* p_args_ret) override {
            using TArgsTuple = std::tuple<TArgs...>;

            constexpr bool has_return = !std::is_void<TRet>::value;
            constexpr auto n_args     = std::tuple_size_v<TArgsTuple>;

            TClass* target = (TClass*) p_object;

#define OFFSET_OF(info) \
    ((std::uint8_t*) p_args_ret + info.stack_offset)

#define GET_ARG(index) *([&]() {                                             \
    using tinfo = RttiArgInfo<std::tuple_element_t<index, TArgsTuple>>;      \
    if constexpr (tinfo::is_ref) {                                           \
        return *((typename tinfo::BaseType**) OFFSET_OF(get_args()[index])); \
    } else if constexpr (tinfo::is_ptr) {                                    \
        return ((typename tinfo::BaseType**) OFFSET_OF(get_args()[index]));  \
    } else {                                                                 \
        return ((typename tinfo::BaseType*) OFFSET_OF(get_args()[index]));   \
    }                                                                        \
}())

#define SET_RET(r)                                                          \
    using tinfo = RttiArgInfo<TRet>;                                        \
    if constexpr (tinfo::is_ref) {                                          \
        *((typename tinfo::BaseType**) OFFSET_OF(get_ret())) = &r;          \
    } else if constexpr (tinfo::is_ptr) {                                   \
        *((typename tinfo::BaseType**) OFFSET_OF(get_ret())) = r;           \
    } else {                                                                \
        *((typename tinfo::BaseType*) OFFSET_OF(get_ret())) = std::move(r); \
    }

#define DO_CALL(...)                                 \
    if constexpr (has_return) {                      \
        TRet r = (*target.*m_p_method)(__VA_ARGS__); \
        SET_RET(r);                                  \
    } else {                                         \
        (*target.*m_p_method)(__VA_ARGS__);          \
    }

            if constexpr (n_args == 0) {
                DO_CALL();
            } else if constexpr (n_args == 1) {
                DO_CALL(GET_ARG(0));
            } else if constexpr (n_args == 2) {
                DO_CALL(GET_ARG(0), GET_ARG(1));
            } else if constexpr (n_args == 3) {
                DO_CALL(GET_ARG(0), GET_ARG(1), GET_ARG(2));
            } else if constexpr (n_args == 4) {
                DO_CALL(GET_ARG(0), GET_ARG(1), GET_ARG(2), GET_ARG(3));
            } else if constexpr (n_args == 5) {
                DO_CALL(GET_ARG(0), GET_ARG(1), GET_ARG(2), GET_ARG(3), GET_ARG(4));
            } else {
                WG_LOG_ERROR("too much arguments count");
                return StatusCode::NotImplemented;
            }

            return WG_OK;

#undef OFFSET_OF
#undef GET_ARG
#undef SET_RET
#undef DO_CALL
        }

    private:
        TSignature m_p_method;
    };

    template<typename ClassT>
    class RttiClassT : public RttiClass {
    public:
        static constexpr int MAX_FUCNTION_ARGS = 5;

        RttiClassT() : RttiClass(ClassT::get_class_name_static(), sizeof(ClassT), ClassT::get_parent_class_static()) {}
        ~RttiClassT() override = default;
        Status consturct(void* dst) const override {
            new (dst) ClassT();
            return WG_OK;
        }
        Status destruct(void* dst) const override {
            ((ClassT*) dst)->~ClassT();
            return WG_OK;
        }
        template<typename T>
        void add_field(Strid name, std::size_t offset_of, RttiMetaData meta_data) {
            RttiField field(name, rtti_type<T>(), sizeof(T), offset_of);
            field.set_metadata(std::move(meta_data));
            RttiClass::add_field(std::move(field));
        }
        template<typename TSignature, typename TRet, typename... TArgs>
        void add_method_t(Strid name, TSignature p_method, const buffered_vector<const char*>& names, RttiMetaData meta_data) {
            using TArgsTuple = std::tuple<TArgs...>;

            constexpr bool has_return = !std::is_void<TRet>::value;
            constexpr auto n_args     = std::tuple_size_v<TArgsTuple>;
            constexpr auto n_names    = has_return ? 1 + n_args : n_args;

            static_assert(n_args <= MAX_FUCNTION_ARGS, "too much arguments");

            if (n_names != n_args) {
                WG_LOG_ERROR("mismatched args names size for" << get_name());
                return;
            }

#define PARAM_INFO(param, str_name)                                                                  \
    param.type         = rtti_type<typename tinfo::BaseType>();                                      \
    param.name         = SID(str_name);                                                              \
    param.is_const     = tinfo::is_const;                                                            \
    param.is_ref       = tinfo::is_ref;                                                              \
    param.is_ptr       = tinfo::is_ptr;                                                              \
    param.stack_offset = stack_size;                                                                 \
    param.stack_size   = param.is_ptr || param.is_ref ? sizeof(void*) : param.type->get_byte_size(); \
    stack_size += param.stack_size;

#define ARG_INFO(index)                                                              \
    if constexpr (index < n_args) {                                                  \
        using tinfo          = RttiArgInfo<std::tuple_element_t<index, TArgsTuple>>; \
        RttiParamInfo& param = args.emplace_back();                                  \
        PARAM_INFO(param, names[index])                                              \
    }

            RttiParamInfo              ret;
            std::vector<RttiParamInfo> args;
            std::size_t                stack_size = 0;

            ARG_INFO(0);
            ARG_INFO(1);
            ARG_INFO(2);
            ARG_INFO(3);
            ARG_INFO(4);

            if constexpr (has_return) {
                using tinfo = RttiArgInfo<TRet>;
                PARAM_INFO(ret, names.back())
            }

            std::stringstream method_name;

            auto print_param = [&](const RttiParamInfo& p) {
                if (!p.type) {
                    method_name << "void";
                    return;
                }

                if (p.is_const) {
                    method_name << "const ";
                }

                method_name << p.type->get_str();

                if (p.is_ptr) {
                    method_name << "*";
                }
                if (p.is_ref) {
                    method_name << "&";
                }

                method_name << " ";
                method_name << p.name.str();
            };

            print_param(ret);
            method_name << " " << get_name().str() << "::";
            method_name << name.str();
            method_name << "(";
            for (const auto& arg : args) {
                print_param(arg);
                method_name << ",";
            }
            method_name << ")";

            RttiMethod method(name, make_ref<RttiFunctionT<ClassT, TSignature, TRet, TArgs...>>(Strid(method_name.str()), p_method, stack_size, std::move(args), std::move(ret)));
            method.set_metadata(std::move(meta_data));
            RttiClass::add_method(std::move(method));

#undef PARAM_INFO
#undef ARG_INFO
        }
        template<typename TRet, typename... TArgs>
        void add_method(Strid name, TRet (ClassT::*p_method)(TArgs...), const buffered_vector<const char*>& names, RttiMetaData meta_data) {
            add_method_t<decltype(p_method), TRet, TArgs...>(name, p_method, names, std::move(meta_data));
        }
        template<typename TRet, typename... TArgs>
        void add_method(Strid name, TRet (ClassT::*p_method)(TArgs...) const, const buffered_vector<const char*>& names, RttiMetaData meta_data) {
            add_method_t<decltype(p_method), TRet, TArgs...>(name, p_method, names, std::move(meta_data));
        }
    };

    template<>
    struct RttiTypeOf<RttiObject> {
        static Strid name() {
            return SID("RttiObject");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiClassT<RttiObject>>();
        }
    };

#define WG_RTTI_GENERATED_TYPE(struct_type, parent_type, rtti_type, rtti_type_getter, modifier)                    \
public:                                                                                                            \
    static Strid get_class_name_static() {                                                                         \
        static Strid g_class_name(#struct_type);                                                                   \
        return g_class_name;                                                                                       \
    }                                                                                                              \
    static Strid get_parent_class_name_static() {                                                                  \
        static Strid g_class_name(#parent_type);                                                                   \
        return g_class_name;                                                                                       \
    }                                                                                                              \
    static rtti_type* get_class_static() {                                                                         \
        static rtti_type* g_class = RttiTypeStorage::instance()->rtti_type_getter(get_class_name_static());        \
        return g_class;                                                                                            \
    }                                                                                                              \
    static rtti_type* get_parent_class_static() {                                                                  \
        static rtti_type* g_class = RttiTypeStorage::instance()->rtti_type_getter(get_parent_class_name_static()); \
        return g_class;                                                                                            \
    }                                                                                                              \
    Strid get_class_name() const modifier {                                                                        \
        return get_class_name_static();                                                                            \
    }                                                                                                              \
    Strid get_parent_class_name() const modifier {                                                                 \
        return get_parent_class_name_static();                                                                     \
    }                                                                                                              \
    rtti_type* get_class() const modifier {                                                                        \
        return get_class_static();                                                                                 \
    }                                                                                                              \
    rtti_type* get_parent_class() const modifier {                                                                 \
        return get_parent_class_static();                                                                          \
    }                                                                                                              \
    friend Status yaml_read(IoContext& context, YamlConstNodeRef node, struct_type& value) {                       \
        return get_class_static()->read_from_yaml(&value, node, context);                                          \
    }                                                                                                              \
    friend Status yaml_write(IoContext& context, YamlNodeRef node, const struct_type& value) {                     \
        return get_class_static()->write_to_yaml(&value, node, context);                                           \
    }                                                                                                              \
    friend Status archive_read(IoContext& context, Archive& archive, struct_type& value) {                         \
        return get_class_static()->read_from_archive(&value, archive, context);                                    \
    }                                                                                                              \
    friend Status archive_write(IoContext& context, Archive& archive, const struct_type& value) {                  \
        return get_class_static()->write_to_archive(&value, archive, context);                                     \
    }                                                                                                              \
    friend struct RttiTypeOf<struct_type>;

#define WG_RTTI_STRUCT_EXT(struct_type, parent_type)                            \
    WG_RTTI_GENERATED_TYPE(struct_type, parent_type, RttiStruct, find_struct, ) \
    std::string to_string() const {                                             \
        RttiStruct*       rtti_type = get_class();                              \
        std::stringstream stream;                                               \
        rtti_type->to_string(this, stream);                                     \
        return stream.str();                                                    \
    }

#define WG_RTTI_STRUCT(struct_type) WG_RTTI_STRUCT_EXT(struct_type, )

#define WG_RTTI_CLASS(class_type, parent_type) WG_RTTI_GENERATED_TYPE(class_type, parent_type, RttiClass, find_class, override)

#define WG_RTTI_STRUCT_BEGIN(struct_type)                             \
    template<>                                                        \
    struct RttiTypeOf<struct_type> {                                  \
        using Type = struct_type;                                     \
        static Strid name() {                                         \
            return SID(#struct_type);                                 \
        }                                                             \
        static Ref<RttiType> make() {                                 \
            Ref<RttiStructT<Type>> t = make_ref<RttiStructT<Type>>(); \
            RttiMetaData           meta_data;                         \
            RttiTypeStorage::instance()->add(t.as<RttiType>());       \
            auto binder = [&]()

#define WG_RTTI_CLASS_BEGIN(class_type)                             \
    template<>                                                      \
    struct RttiTypeOf<class_type> {                                 \
        using Type = class_type;                                    \
        static Strid name() {                                       \
            return SID(#class_type);                                \
        }                                                           \
        static Ref<RttiType> make() {                               \
            Ref<RttiClassT<Type>> t = make_ref<RttiClassT<Type>>(); \
            RttiMetaData          meta_data;                        \
            RttiTypeStorage::instance()->add(t.as<RttiType>());     \
            auto binder = [&]()

#define WG_RTTI_END                        \
    ;                                      \
    binder();                              \
    t->set_metadata(std::move(meta_data)); \
    return t.as<RttiType>();               \
    }                                      \
    }

#define WG_RTTI_META_DATA(...) \
    meta_data = std::move(RttiMetaData({__VA_ARGS__}))

#define WG_RTTI_FACTORY() \
    t->add_factory([]() -> RttiObject* { return new Type(); })

#define WG_RTTI_FIELD(property, ...) \
    t->add_field<decltype(Type::property)>(SID(#property), offsetof(Type, property), __VA_ARGS__)

#define WG_RTTI_METHOD(method, ...) \
    t->add_method(SID(#method), &Type::method, __VA_ARGS__)

}// namespace wmoge