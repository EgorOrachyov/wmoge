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

#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/typed_array.hpp"
#include "core/typed_map.hpp"
#include "io/stream.hpp"
#include "io/tree.hpp"
#include "math/math_utils.hpp"
#include "math/vec.hpp"

#include <cinttypes>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace wmoge {

    /**
     * @brief Types supported for storing inside a variant
     */
    enum class VarType : int {
        Nil         = 0, // Null or empty value type
        Bool        = 1, // Boolean value type
        Int         = 2, // Integer number type
        Float       = 3, // Floating point number type
        String      = 4, // String type
        Strid       = 5, // Strid (string id) type
        Array       = 6, // Array (dynamic vector) of var values
        Map         = 7, // Map of var to var value
        ArrayInt    = 8, // Array of int values
        ArrayFloat  = 9, // Array of float values
        ArrayByte   = 10,// Array of byte (uint8) value
        ArrayString = 11,// Array of strings
        ArrayVec2f  = 12,// Array of vec2f values
        ArrayVec3f  = 13 // Array of vec3f values
    };

    /** 
     * @class VarHash
     * @brief Variant hasher for unordered maps
     */
    struct VarHash final {
        std::size_t operator()(const class Var&) const;
    };

    class Var;
    using String      = std::string;
    using Array       = TypedArray<Var>;            // wrap to avoid unnecessary copy
    using ArrayInt    = TypedArray<int>;            // wrap to avoid unnecessary copy
    using ArrayFloat  = TypedArray<float>;          // wrap to avoid unnecessary copy
    using ArrayByte   = TypedArray<std::uint8_t>;   // wrap to avoid unnecessary copy
    using ArrayString = TypedArray<String>;         // wrap to avoid unnecessary copy
    using ArrayVec2f  = TypedArray<Vec2f>;          // wrap to avoid unnecessary copy
    using ArrayVec3f  = TypedArray<Vec3f>;          // wrap to avoid unnecessary copy
    using Map         = TypedMap<Var, Var, VarHash>;// wrap to avoid unnecessary copy

    /**
     * @class Var
     * @brief Universal resalable variant value with support for common engine types and containers
     */
    class Var {
    public:
        Var() = default;
        Var(bool value);
        Var(long long value);
        Var(int value);
        Var(double value);
        Var(String value);
        Var(Strid value);
        Var(Array value);
        Var(Map value);
        Var(std::size_t value);
        Var(std::vector<Var> value);
        Var(std::unordered_map<Var, Var, VarHash> value);
        Var(ArrayByte value);
        Var(ArrayInt value);
        Var(ArrayFloat value);
        Var(ArrayString value);
        Var(ArrayVec2f value);
        Var(ArrayVec3f value);

        template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
        Var(T enum_value) : Var(static_cast<int>(enum_value)) {}

        Var(const Var& var);
        Var(Var&& var) noexcept;
        ~Var();

        Var& operator=(const Var& var);
        Var& operator=(Var&& var) noexcept;

        bool operator==(const Var& var) const;
        bool operator!=(const Var& var) const;
        bool operator<(const Var& var) const;

        operator bool() const;
        operator long long() const;
        operator int() const;
        operator float() const;
        operator double() const;
        operator std::size_t() const;
        operator String() const;
        operator Strid() const;
        operator Array() const;
        operator Map() const;
        operator ArrayByte() const;
        operator ArrayInt() const;
        operator ArrayFloat() const;
        operator ArrayString() const;
        operator ArrayVec2f() const;
        operator ArrayVec3f() const;

        VarType type() const { return m_type; }

        std::string to_string() const;
        std::size_t hash() const;

    private:
        template<typename T>
        const T& as() const { return *reinterpret_cast<const T*>(m_data.m_mem); }
        template<typename T>
        T& as() { return *reinterpret_cast<T*>(m_data.m_mem); }

        void release();
        void build_string(std::stringstream& stream) const;
        void build_hash(std::size_t& accum) const;

    private:
        static constexpr std::size_t MEM_SIZE =
                Math::const_max<size_t,
                                sizeof(void*),
                                sizeof(Strid),
                                sizeof(String)>();

        VarType m_type = VarType::Nil;

        union {
            bool         m_bool;
            long long    m_int;
            double       m_float;
            std::uint8_t m_mem[MEM_SIZE] = {0};
        } m_data;
    };

    inline std::ostream& operator<<(std::ostream& stream, const wmoge::Var& var) {
        stream << var.to_string();
        return stream;
    }

    Status tree_read(IoContext& context, IoTree& tree, Var& value);
    Status tree_write(IoContext& context, IoTree& tree, const Var& value);

    Status stream_read(IoContext& context, IoStream& stream, Var& value);
    Status stream_write(IoContext& context, IoStream& stream, const Var& value);

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::Var> {
    public:
        std::size_t operator()(const wmoge::Var& var) const {
            return var.hash();
        }
    };

}// namespace std