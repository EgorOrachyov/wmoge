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
#include "core/var.hpp"
#include "io/enum.hpp"
#include "io/stream.hpp"
#include "io/tree.hpp"
#include "math/aabb.hpp"
#include "math/mat.hpp"
#include "math/quat.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"
#include "rtti/builtin.hpp"
#include "rtti/class.hpp"
#include "rtti/enum.hpp"
#include "rtti/object.hpp"
#include "rtti/struct.hpp"
#include "rtti/type.hpp"
#include "rtti/type_storage.hpp"

#include <bitset>
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
        const Strid      rtti_name = SID(RttiTypeOf<T>::name());
        static RttiType* g_type    = RttiTypeStorage::instance()->find_type(rtti_name).value_or(nullptr);

        if (!g_type) {
            g_type = RttiTypeStorage::instance()->find_type(rtti_name).value_or(nullptr);

            if (!g_type) {
                Ref<RttiType> type = RttiTypeOf<T>::make();
                RttiTypeStorage::instance()->add(type);
                g_type = type.get();

                if constexpr (std::is_base_of_v<RttiObject, T>) {
                    RttiTypeOf<T>::bind(type);
                }
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
        Status clone(void* dst, const void* src) const override {
            *((T*) dst) = *((const T*) src);
            return WG_OK;
        }
        Status destruct(void* dst) const override {
            ((T*) dst)->~T();
            return WG_OK;
        }
        Status read_from_tree(void* dst, IoTree& tree, IoContext& context) const override {
            return tree_read(context, tree, *((T*) dst));
        }
        Status write_to_tree(const void* src, IoTree& tree, IoContext& context) const override {
            return tree_write(context, tree, *((const T*) src));
        }
        Status read_from_stream(void* dst, IoStream& stream, IoContext& context) const override {
            return stream_read(context, stream, *((T*) dst));
        }
        Status write_to_stream(const void* src, IoStream& stream, IoContext& context) const override {
            return stream_write(context, stream, *((T*) src));
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
        Status clone(void* dst, const void* src) const override {
            const PairT& source = *((const PairT*) src);
            PairT&       target = *((PairT*) dst);
            WG_CHECKED(RttiTypePair::m_key_type->clone(&(target.first), &(source.first)));
            WG_CHECKED(RttiTypePair::m_value_type->clone(&(target.second), &(source.second)));
            return WG_OK;
        }
        Status to_string(const void* src, std::stringstream& s) const override {
            const PairT& pair = *((const PairT*) src);
            s << "{";
            RttiTypePair::m_key_type->to_string(&(pair.first), s);
            s << ":";
            RttiTypePair::m_value_type->to_string(&(pair.second), s);
            s << "}";
            return WG_OK;
        }
        Status visit(void* src, const std::function<Status(const void*, const void*)>& func) const override {
            PairT& self = *((PairT*) src);
            WG_CHECKED(func(&(self.first), &(self.second)));
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

    template<typename BitsetT, int N>
    class RttiTypeBitsetT : public RttiTypeT<BitsetT, RttiTypeBitset> {
    public:
        RttiTypeBitsetT(Strid name) : RttiTypeT<BitsetT, RttiTypeBitset>(name) {
            RttiTypeBitset::m_dimension = N;
        }
        ~RttiTypeBitsetT() override = default;
        Status to_string(const void* src, std::stringstream& s) const override {
            s << *((const BitsetT*) src);
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
        Status clone(void* dst, const void* src) const override {
            const VecT& source = *((const VecT*) src);
            VecT&       target = *((VecT*) dst);
            target.clear();
            target.resize(source.size());
            for (std::size_t i = 0; i < target.size(); i++) {
                WG_CHECKED(RttiTypeVector::m_value_type->clone(&target[i], &source[i]));
            }
            return WG_OK;
        }
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
        Status push_back(void* src, void* value) const override {
            VecT&        vec  = *((VecT*) src);
            const ElemT& elem = *((const ElemT*) value);
            vec.push_back(elem);
            return WG_OK;
        }
        Status iterate(void* src, const std::function<Status(const void*)>& func) const override {
            VecT& vec = *((VecT*) src);
            for (auto& elem : vec) {
                WG_CHECKED(func(&elem));
            }
            return WG_OK;
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
        Status clone(void* dst, const void* src) const override {
            const MapT& source = *((const MapT*) src);
            MapT&       target = *((MapT*) dst);
            target.clear();
            target.reserve(source.size());
            for (const auto& source_entry : source) {
                KeyT   key;
                ValueT val;
                WG_CHECKED(RttiTypeMap::m_key_type->clone(&key, &(source_entry.first)));
                WG_CHECKED(RttiTypeMap::m_value_type->clone(&val, &(source_entry.second)));
                target.emplace(std::move(key), std::move(val));
            }
            return WG_OK;
        }
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
        Status iterate(void* src, const std::function<Status(const void*, const void*)>& func) const override {
            MapT& map = *((MapT*) src);
            for (auto& elem : map) {
                WG_CHECKED(func(&(elem.first), &(elem.second)));
            }
            return WG_OK;
        }
    };

    template<typename SetT, typename KeyT>
    class RttiTypeBaseSetT : public RttiTypeT<SetT, RttiTypeSet> {
    public:
        RttiTypeBaseSetT(Strid name) : RttiTypeT<SetT, RttiTypeSet>(name) {
            RttiTypeSet::m_value_type = rtti_type<KeyT>();
        }
        ~RttiTypeBaseSetT() override = default;
        Status clone(void* dst, const void* src) const override {
            const SetT& source = *((const SetT*) src);
            SetT&       target = *((SetT*) dst);
            target.clear();
            target.reserve(source.size());
            for (const auto& source_entry : source) {
                KeyT key;
                WG_CHECKED(RttiTypeSet::m_value_type->clone(&key, &source_entry));
                target.emplace(std::move(key));
            }
            return WG_OK;
        }
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
        Status iterate(void* src, const std::function<Status(const void*)>& func) const override {
            SetT& set = *((SetT*) src);
            for (auto& elem : set) {
                WG_CHECKED(func(&elem));
            }
            return WG_OK;
        }
    };

    template<typename HolderT, typename PtrT, typename BaseRefType>
    class RttiTypeRefT : public RttiTypeT<HolderT, BaseRefType> {
    public:
        static_assert(std::is_base_of_v<RttiObject, PtrT>, "Must be an rtti object base type");

        using ParentT = RttiTypeT<HolderT, BaseRefType>;
        using RefT    = Ref<PtrT>;

        RttiTypeRefT(Strid name) : ParentT(name) {
            BaseRefType::m_value_type = rtti_type<PtrT>();
        }
        ~RttiTypeRefT() override = default;
        Status clone(void* dst, const void* src) const override {
            const RefT& source = *((const RefT*) src);
            RefT&       target = *((RefT*) dst);
            if (!source) {
                target = source;
                return WG_OK;
            }
            const RttiClass* cls = source->get_class();
            target               = cls->instantiate().cast<PtrT>();
            if (!target) {
                return StatusCode::FailedInstantiate;
            }
            WG_CHECKED(cls->clone(target.get(), source.get()));
            return WG_OK;
        }
        Status to_string(const void* src, std::stringstream& s) const override {
            const RefT& self = *((const RefT*) src);
            if (self) {
                return BaseRefType::m_value_type->to_string(self.get(), s);
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
        Status clone(void* dst, const void* src) const override {
            const OptionalT& source = *((const OptionalT*) src);
            OptionalT&       target = *((OptionalT*) dst);
            if (source) {
                T val;
                WG_CHECKED(RttiTypeOptional::m_value_type->clone(&val, &source.value()));
                target = std::move(val);
            }
            return WG_OK;
        }
        Status to_string(const void* src, std::stringstream& s) const override {
            const OptionalT& self = *((const OptionalT*) src);
            if (self) {
                return RttiTypeOptional::m_value_type->to_string(&self.value(), s);
            } else {
                s << "nil";
            }
            return WG_OK;
        }
        Status set_value(void* dst, const void* src) const override {
            OptionalT& self = *((OptionalT*) src);
            const T&   val  = *((const T*) src);
            self            = val;
            return WG_OK;
        }
        Status visit(void* src, const std::function<Status(const void*)>& func) const override {
            OptionalT& self = *((OptionalT*) src);
            if (self) {
                WG_CHECKED(func(&(self.value())));
            }
            return WG_OK;
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

    template<typename FuncT, typename TRet, typename... TArgs>
    struct RttiTypeFunctionBaseT : public RttiTypeFunction {
        static constexpr int  MAX_ARGS_COUNT  = 5;
        static constexpr bool HAS_RETURN_TYPE = !std::is_void<TRet>::value;

        using ParentT   = RttiTypeFunction;
        using FunctionT = FuncT;

        RttiTypeFunctionBaseT(Strid name, std::size_t stack_size, std::vector<RttiParamInfo> args, RttiParamInfo ret)
            : RttiTypeFunction(name, sizeof(FunctionT), stack_size, std::move(args), std::move(ret)) {
        }
        Status consturct(void* dst) const override {
            new (dst) FunctionT();
            return WG_OK;
        }
        Status copy(void* dst, const void* src) const override {
            *((FunctionT*) dst) = *((const FunctionT*) src);
            return WG_OK;
        }
        Status clone(void* dst, const void* src) const override {
            *((FunctionT*) dst) = *((const FunctionT*) src);
            return WG_OK;
        }
        Status destruct(void* dst) const override {
            ((FunctionT*) dst)->~FunctionT();
            return WG_OK;
        }
        Status to_string(const void* src, std::stringstream& s) const override {
            const FunctionT& self = *((const FunctionT*) src);
            if (self) {
                s << "<body>";
            } else {
                s << "null";
            }
            return WG_OK;
        }

        template<typename Performer>
        Status perform_call(Performer& performer, RttiFrame& frame, void* target, array_view<std::uint8_t> args_ret) const {
            using TArgsTuple = std::tuple<TArgs...>;

            constexpr bool has_return = !std::is_void<TRet>::value;
            constexpr auto n_args     = std::tuple_size_v<TArgsTuple>;

            void* p_args_ret = args_ret.data();

            if (!target) {
                WG_LOG_ERROR("null target passed to call " << RttiTypeFunction::get_name());
                return StatusCode::InvalidParameter;
            }
            if (args_ret.size() != m_stack_size) {
                WG_LOG_ERROR("invalid args_ret passed to call " << RttiTypeFunction::get_name());
                return StatusCode::InvalidParameter;
            }

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

#define DO_CALL(...)                         \
    do {                                     \
        if constexpr (has_return) {          \
            TRet r = performer(__VA_ARGS__); \
            SET_RET(r);                      \
        } else {                             \
            performer(__VA_ARGS__);          \
        }                                    \
    } while (0)

            if constexpr (n_args == 0) {
                DO_CALL(target);
            } else if constexpr (n_args == 1) {
                DO_CALL(target, GET_ARG(0));
            } else if constexpr (n_args == 2) {
                DO_CALL(target, GET_ARG(0), GET_ARG(1));
            } else if constexpr (n_args == 3) {
                DO_CALL(target, GET_ARG(0), GET_ARG(1), GET_ARG(2));
            } else if constexpr (n_args == 4) {
                DO_CALL(target, GET_ARG(0), GET_ARG(1), GET_ARG(2), GET_ARG(3));
            } else if constexpr (n_args == 5) {
                DO_CALL(target, GET_ARG(0), GET_ARG(1), GET_ARG(2), GET_ARG(3), GET_ARG(4));
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

        static void fill_param_info(std::vector<RttiParamInfo>& args, RttiParamInfo& ret, std::size_t& stack_size) {
            using TArgsTuple = std::tuple<TArgs...>;

            constexpr bool has_return = !std::is_void<TRet>::value;
            constexpr auto n_args     = std::tuple_size_v<TArgsTuple>;

            static_assert(n_args <= MAX_ARGS_COUNT, "too much arguments");

#define PARAM_INFO(param)                                                                            \
    param.type         = rtti_type<typename tinfo::BaseType>();                                      \
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
        PARAM_INFO(param)                                                            \
    }

            ARG_INFO(0);
            ARG_INFO(1);
            ARG_INFO(2);
            ARG_INFO(3);
            ARG_INFO(4);

            if constexpr (has_return) {
                using tinfo = RttiArgInfo<TRet>;
                PARAM_INFO(ret)
            }

#undef PARAM_INFO
#undef ARG_INFO
        }
    };

    template<typename FuncT, typename TRet, typename... TArgs>
    class RttiTypeFunctionLambaT : public RttiTypeFunctionBaseT<FuncT, TRet, TArgs...> {
    public:
        using ParentT = RttiTypeFunctionBaseT<FuncT, TRet, TArgs...>;

        RttiTypeFunctionLambaT(Strid name, std::size_t stack_size, std::vector<RttiParamInfo> args, RttiParamInfo ret)
            : ParentT(name, stack_size, std::move(args), std::move(ret)) {
        }

        virtual Status call(RttiFrame& frame, void* target, array_view<std::uint8_t> args_ret) const {
            struct Performer {
                TRet operator()(void* target, TArgs... args) const {
                    const FuncT& self = *((FuncT*) target);

                    if constexpr (ParentT::HAS_RETURN_TYPE) {
                        return self(args...);
                    } else {
                        self(args...);
                    }
                }
            };

            Performer performer;
            return ParentT::perform_call(performer, frame, target, args_ret);
        }
    };

    template<typename ClassT, typename SignatureT, typename TRet, typename... TArgs>
    class RttiTypeClassMethodT : public RttiTypeFunctionBaseT<void*, TRet, TArgs...> {
    public:
        using ParentT = RttiTypeFunctionBaseT<void*, TRet, TArgs...>;

        RttiTypeClassMethodT(Strid name, SignatureT p_method, std::size_t stack_size, std::vector<RttiParamInfo> args, RttiParamInfo ret)
            : ParentT(name, stack_size, std::move(args), std::move(ret)), m_p_method(p_method) {
        }

        Status call(RttiFrame& frame, void* target, array_view<std::uint8_t> args_ret) const override {
            struct Performer {
                SignatureT p_method;

                Performer(SignatureT p_method) : p_method(p_method) {
                }

                TRet operator()(void* target, TArgs... args) const {
                    ClassT* self = (ClassT*) target;

                    if constexpr (ParentT::HAS_RETURN_TYPE) {
                        return (*self.*p_method)(args...);
                    } else {
                        (*self.*p_method)(args...);
                    }
                }
            };

            Performer performer(m_p_method);
            return ParentT::perform_call(performer, frame, target, args_ret);
        }

    private:
        SignatureT m_p_method;
    };

    template<typename T>
    struct RttiTypeOf<Ref<T>, typename std::enable_if<std::is_base_of_v<RttiObject, T>>::type> {
        static Strid name() {
            return SID(std::string("ref<") + rtti_type<T>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeRefT<Ref<T>, T, RttiTypeRef>>(name());
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

    WG_RTTI_FUNDAMENTAL_DECL(Var, "var");
    WG_RTTI_FUNDAMENTAL_DECL(std::int16_t, "int16_t");
    WG_RTTI_FUNDAMENTAL_DECL(int, "int");
    WG_RTTI_FUNDAMENTAL_DECL(unsigned int, "uint");
    WG_RTTI_FUNDAMENTAL_DECL(float, "float");
    WG_RTTI_FUNDAMENTAL_DECL(bool, "bool");
    WG_RTTI_FUNDAMENTAL_DECL(std::size_t, "size_t");
    WG_RTTI_FUNDAMENTAL_DECL(std::string, "string");
    WG_RTTI_FUNDAMENTAL_DECL(Strid, "strid");
    WG_RTTI_FUNDAMENTAL_DECL(UUID, "uuid");
    WG_RTTI_FUNDAMENTAL_DECL(Sha256, "sha256");
    WG_RTTI_FUNDAMENTAL_DECL(DateTime, "datetime");
    WG_RTTI_FUNDAMENTAL_DECL(Status, "status");
    WG_RTTI_FUNDAMENTAL_DECL(Ref<Data>, "data");
    WG_RTTI_FUNDAMENTAL_DECL(Aabbf, "aabbf");
    WG_RTTI_FUNDAMENTAL_DECL(Transform2d, "transform2d");
    WG_RTTI_FUNDAMENTAL_DECL(Transform3d, "transform3d");
    WG_RTTI_FUNDAMENTAL_DECL(TransformEdt, "transformEdt");

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

    template<std::size_t N>
    struct RttiTypeOf<std::bitset<N>> {
        using BitsetType = std::bitset<N>;

        static Strid name() {
            return SID(std::string("bitset<") + std::to_string(N) + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeBitsetT<BitsetType, int(N)>>(name());
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

    template<typename K>
    struct RttiTypeOf<robin_hood::unordered_flat_set<K>> {
        using SetT = robin_hood::unordered_flat_set<K>;

        static Strid name() {
            return SID(std::string("fset<") + rtti_type<K>()->get_str() + ">");
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

    template<typename TRet, typename... TArgs>
    struct RttiTypeOf<std::function<TRet(TArgs...)>> {
        using FunctionT = std::function<TRet(TArgs...)>;
        using TraitType = RttiTypeFunctionLambaT<FunctionT, TRet, TArgs...>;

        static Strid name() {
            RttiParamInfo              ret;
            std::vector<RttiParamInfo> args;
            std::size_t                stack_size = 0;
            TraitType::fill_param_info(args, ret, stack_size);

            std::stringstream function_name;
            function_name << "function<";

            ret.print_param(function_name);
            function_name << "(";
            for (const auto& arg : args) {
                arg.print_param(function_name);
                function_name << ",";
            }
            function_name << ")>";

            return SID(function_name.str());
        }

        static Ref<RttiType> make() {
            RttiParamInfo              ret;
            std::vector<RttiParamInfo> args;
            std::size_t                stack_size = 0;
            TraitType::fill_param_info(args, ret, stack_size);

            return make_ref<TraitType>(name(), stack_size, std::move(args), std::move(ret));
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
            using TraitType  = RttiTypeClassMethodT<ClassT, TSignature, TRet, TArgs...>;

            constexpr bool has_return = !std::is_void<TRet>::value;
            constexpr auto n_args     = std::tuple_size_v<TArgsTuple>;

            static_assert(n_args <= MAX_FUCNTION_ARGS, "too much arguments");

            if (names.size() != n_args) {
                WG_LOG_ERROR("mismatched args names size for" << get_name());
                return;
            }

            RttiParamInfo              ret;
            std::vector<RttiParamInfo> args;
            std::size_t                stack_size = 0;
            TraitType::fill_param_info(args, ret, stack_size);

            for (std::size_t i = 0; i < n_args; i++) {
                args[i].name = SID(names[i]);
            }

            std::stringstream method_name;

            ret.print_param(method_name);
            method_name << " " << get_name().str() << "::";
            method_name << name.str();
            method_name << "(";
            for (const auto& arg : args) {
                arg.print_param(method_name);
                method_name << ",";
            }
            method_name << ")";

            RttiMethod method(name, make_ref<TraitType>(Strid(method_name.str()), p_method, stack_size, std::move(args), std::move(ret)));
            method.set_metadata(std::move(meta_data));
            RttiClass::add_method(std::move(method));
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
        static Ref<RttiType> bind(const Ref<RttiType>& type) {
            return type;
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
    static const std::string& get_extension_static() {                                                             \
        return get_class_static()->get_extension();                                                                \
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
    const std::string& get_extension() const modifier {                                                            \
        return get_class()->get_extension();                                                                       \
    }                                                                                                              \
    friend Status tree_read(IoContext& context, IoTree& tree, struct_type& value) {                                \
        return get_class_static()->read_from_tree(&value, tree, context);                                          \
    }                                                                                                              \
    friend Status tree_write(IoContext& context, IoTree& tree, const struct_type& value) {                         \
        return get_class_static()->write_to_tree(&value, tree, context);                                           \
    }                                                                                                              \
    friend Status stream_read(IoContext& context, IoStream& stream, struct_type& value) {                          \
        return get_class_static()->read_from_stream(&value, stream, context);                                      \
    }                                                                                                              \
    friend Status stream_write(IoContext& context, IoStream& stream, const struct_type& value) {                   \
        return get_class_static()->write_to_stream(&value, stream, context);                                       \
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

#define WG_RTTI_CLASS_BEGIN(class_type)                                   \
    template<>                                                            \
    struct RttiTypeOf<class_type> {                                       \
        using Type = class_type;                                          \
        static Strid name() {                                             \
            return SID(#class_type);                                      \
        }                                                                 \
        static Ref<RttiType> make() {                                     \
            Ref<RttiClassT<Type>> t = make_ref<RttiClassT<Type>>();       \
            return t.as<RttiType>();                                      \
        }                                                                 \
        static Ref<RttiType> bind(const Ref<RttiType>& type) {            \
            RttiMetaData          meta_data;                              \
            Ref<RttiClassT<Type>> t      = type.cast<RttiClassT<Type>>(); \
            auto                  binder = [&]()

#define WG_RTTI_END                        \
    ;                                      \
    binder();                              \
    t->set_metadata(std::move(meta_data)); \
    return t.as<RttiType>();               \
    }                                      \
    }

#define WG_RTTI_META_DATA(...) \
    meta_data = std::move(RttiMetaData(__VA_ARGS__))

#define WG_RTTI_FACTORY() \
    t->add_factory([]() -> RttiObject* { return new Type(); })

#define WG_RTTI_EXTENSION(ext) \
    t->set_extension(ext);

#define WG_RTTI_FIELD(property, ...) \
    t->add_field<decltype(Type::property)>(SID(#property), offsetof(Type, property), __VA_ARGS__)

#define WG_RTTI_METHOD(method, ...) \
    t->add_method(SID(#method), &Type::method, __VA_ARGS__)

}// namespace wmoge