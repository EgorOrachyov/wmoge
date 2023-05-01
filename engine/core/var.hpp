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

#ifndef WMOGE_VAR_HPP
#define WMOGE_VAR_HPP

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "math/math_utils.hpp"

namespace wmoge {

    enum class VarType : int {
        Nil      = 0,
        Int      = 1,
        Float    = 2,
        String   = 3,
        StringId = 4,
        Array    = 5,
        Map      = 6,
        Object   = 7
    };

    class Var;
    class Object;
    using vvector = std::vector<Var>;
    using vmap    = std::map<Var, Var>;
    using String  = std::string;
    using Array   = std::shared_ptr<vvector>;// wrap to avoid unnecessary copy
    using Map     = std::shared_ptr<vmap>;   // wrap to avoid unnecessary copy

    /**
     * @class Var
     * @brief Universal resalable variant value with support for common engine types and containers
     */
    class Var {
    public:
        Var() = default;
        Var(long long value);
        Var(int value);
        Var(double value);
        Var(String value);
        Var(StringId value);
        Var(Array value);
        Var(Map value);
        Var(const ref_ptr<Object>& value);
        Var(std::size_t value);
        Var(std::vector<Var> value);
        Var(std::map<Var, Var> value);

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
        operator StringId() const;
        operator Array() const;
        operator Map() const;
        operator ref_ptr<Object>() const;

        VarType type() const { return m_type; }

        std::string to_string() const;
        std::size_t hash() const;

    private:
        const String*   as_string() const;
        const StringId* as_string_id() const;
        const Array*    as_array() const;
        const Map*      as_map() const;

        String*   as_string();
        StringId* as_string_id();
        Array*    as_array();
        Map*      as_map();

        void release();
        void build_string(std::stringstream& stream) const;
        void build_hash(std::size_t& accum) const;

    private:
        static constexpr std::size_t MEM_SIZE =
                Math::const_max<size_t,
                                sizeof(Array),
                                sizeof(Map),
                                sizeof(StringId),
                                sizeof(String)>();

        VarType m_type = VarType::Nil;

        union {
            long long    m_int;
            double       m_float;
            Object*      m_object;
            std::uint8_t m_mem[MEM_SIZE] = {0};
        } m_data;
    };

    inline std::ostream& operator<<(std::ostream& stream, const wmoge::Var& var) {
        stream << var.to_string();
        return stream;
    }

}// namespace wmoge

#endif//WMOGE_VAR_HPP
