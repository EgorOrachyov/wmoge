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

#include "var.hpp"
#include "object.hpp"

#include <algorithm>
#include <utility>

namespace wmoge {

    const String* Var::as_string() const {
        return reinterpret_cast<const String*>(m_data.m_mem);
    }
    const StringId* Var::as_string_id() const {
        return reinterpret_cast<const StringId*>(m_data.m_mem);
    }
    const Array* Var::as_array() const {
        return reinterpret_cast<const Array*>(m_data.m_mem);
    }
    const Map* Var::as_map() const {
        return reinterpret_cast<const Map*>(m_data.m_mem);
    }

    String* Var::as_string() {
        return reinterpret_cast<String*>(m_data.m_mem);
    }
    StringId* Var::as_string_id() {
        return reinterpret_cast<StringId*>(m_data.m_mem);
    }
    Array* Var::as_array() {
        return reinterpret_cast<Array*>(m_data.m_mem);
    }
    Map* Var::as_map() {
        return reinterpret_cast<Map*>(m_data.m_mem);
    }

    Var::Var(long long value) {
        m_type       = VarType::Int;
        m_data.m_int = value;
    }
    Var::Var(int value) {
        m_type       = VarType::Int;
        m_data.m_int = value;
    }
    Var::Var(double value) {
        m_type         = VarType::Float;
        m_data.m_float = value;
    }
    Var::Var(std::string value) {
        m_type = VarType::String;
        new (m_data.m_mem) String(std::move(value));
    }
    Var::Var(StringId value) {
        m_type = VarType::StringId;
        new (m_data.m_mem) StringId(std::move(value));
    }
    Var::Var(Array value) {
        m_type = VarType::Array;
        new (m_data.m_mem) Array(std::move(value));
    }
    Var::Var(Map value) {
        m_type = VarType::Map;
        new (m_data.m_mem) Map(std::move(value));
    }
    Var::Var(const Ref<Object>& value) {
        m_type          = VarType::Object;
        m_data.m_object = ref(value.get());
    }
    Var::Var(std::size_t value) : Var(static_cast<long long>(value)) {
    }
    Var::Var(std::vector<Var> value) : Var(std::make_shared<std::vector<Var>>(std::move(value))) {
    }
    Var::Var(std::map<Var, Var> value) : Var(std::make_shared<std::map<Var, Var>>(std::move(value))) {
    }

    Var::Var(const Var& var) {
        switch (var.type()) {
            case VarType::Int:
                m_data.m_int = var.m_data.m_int;
                break;
            case VarType::Float:
                m_data.m_float = var.m_data.m_float;
                break;
            case VarType::String:
                new (m_data.m_mem) String(*var.as_string());
                break;
            case VarType::StringId:
                new (m_data.m_mem) StringId(*var.as_string_id());
                break;
            case VarType::Array:
                new (m_data.m_mem) Array(*var.as_array());
                break;
            case VarType::Map:
                new (m_data.m_mem) Map(*var.as_map());
                break;
            case VarType::Object:
                m_data.m_object = ref(var.m_data.m_object);
                break;
            default:
                break;
        }

        m_type = var.type();
    }

    Var::Var(Var&& var) noexcept {
        switch (var.type()) {
            case VarType::Int:
                m_data.m_int = var.m_data.m_int;
                break;
            case VarType::Float:
                m_data.m_float = var.m_data.m_float;
                break;
            case VarType::String:
                new (m_data.m_mem) String(std::move(*var.as_string()));
                break;
            case VarType::StringId:
                new (m_data.m_mem) StringId(std::move(*var.as_string_id()));
                break;
            case VarType::Array:
                new (m_data.m_mem) Array(std::move(*var.as_array()));
                break;
            case VarType::Map:
                new (m_data.m_mem) Map(std::move(*var.as_map()));
                break;
            case VarType::Object:
                m_data.m_object = ref(var.m_data.m_object);
                break;
            default:
                break;
        }

        m_type = var.type();
        var.release();
    }

    Var::~Var() {
        release();
    }

    Var& Var::operator=(const Var& var) {
        if (this != &var) {
            release();
            new (this) Var(var);
        }

        return *this;
    }
    Var& Var::operator=(Var&& var) noexcept {
        if (this != &var) {
            release();
            new (this) Var(std::move(var));
        }

        return *this;
    }

    bool Var::operator==(const Var& var) const {
        if (type() != var.type()) {
            return false;
        }

        switch (type()) {
            case VarType::Nil:
                return true;
            case VarType::Int:
                return m_data.m_int == var.m_data.m_int;
            case VarType::Float:
                return Math::abs(m_data.m_float - var.m_data.m_float) <= Math::THRESH_COMPARE_FLOAT32;
            case VarType::String:
                return *as_string() == *var.as_string();
            case VarType::StringId:
                return *as_string_id() == *var.as_string_id();
            case VarType::Array: {
                const auto& ar1 = *as_array()->get();
                const auto& ar2 = *var.as_array()->get();
                return (ar1.size() == ar2.size()) && std::equal(ar1.begin(), ar1.end(), ar2.begin());
            }
            case VarType::Map: {
                const auto& mp1 = *as_map()->get();
                const auto& mp2 = *var.as_map()->get();
                return (mp1.size() == mp2.size()) && std::equal(mp1.begin(), mp1.end(), mp2.begin());
            }
            case VarType::Object:
                return m_data.m_object == var.m_data.m_object;
            default:
                return false;
        }
    }

    bool Var::operator!=(const Var& var) const {
        if (type() != var.type()) {
            return true;
        }

        switch (type()) {
            case VarType::Nil:
                return false;
            case VarType::Int:
                return m_data.m_int != var.m_data.m_int;
            case VarType::Float:
                return Math::abs(m_data.m_float - var.m_data.m_float) > Math::THRESH_COMPARE_FLOAT32;
            case VarType::String:
                return *as_string() != *var.as_string();
            case VarType::StringId:
                return *as_string_id() != *var.as_string_id();
            case VarType::Array: {
                const auto& ar1 = *as_array()->get();
                const auto& ar2 = *var.as_array()->get();
                return (ar1.size() != ar2.size()) || !std::equal(ar1.begin(), ar1.end(), ar2.begin());
            }
            case VarType::Map: {
                const auto& mp1 = *as_map()->get();
                const auto& mp2 = *var.as_map()->get();
                return (mp1.size() != mp2.size()) || !std::equal(mp1.begin(), mp1.end(), mp2.begin());
            }
            case VarType::Object:
                return m_data.m_object != var.m_data.m_object;
            default:
                return true;
        }
    }

    bool Var::operator<(const Var& var) const {
        if (type() != var.type()) {
            return static_cast<int>(type()) < static_cast<int>(var.type());
        }

        switch (type()) {
            case VarType::Nil:
                return false;
            case VarType::Int:
                return m_data.m_int < var.m_data.m_int;
            case VarType::Float:
                return m_data.m_float < var.m_data.m_float;
            case VarType::String:
                return *as_string() < *var.as_string();
            case VarType::StringId:
                return *as_string_id() < *var.as_string_id();
            case VarType::Array: {
                const auto& ar1 = *as_array()->get();
                const auto& ar2 = *var.as_array()->get();
                if (ar1.size() != ar2.size()) {
                    return ar1.size() < ar2.size();
                }
                auto it1 = ar1.begin();
                auto it2 = ar2.begin();
                while (it1 != ar1.end()) {
                    if (*it1 < *it2) {
                        return true;
                    }
                    if (*it1 != *it2) {
                        return false;
                    }
                    ++it1;
                    ++it2;
                }
                return false;
            }
            case VarType::Map: {
                const auto& mp1 = *as_map()->get();
                const auto& mp2 = *var.as_map()->get();
                if (mp1.size() != mp2.size()) {
                    return mp1.size() < mp2.size();
                }
                auto it1 = mp1.begin();
                auto it2 = mp2.begin();
                while (it1 != mp1.end()) {
                    if (*it1 < *it2) {
                        return true;
                    }
                    if (*it1 != *it2) {
                        return false;
                    }
                    ++it1;
                    ++it2;
                }
                return false;
            }
            case VarType::Object:
                return m_data.m_object < var.m_data.m_object;
            default:
                return false;
        }
    }

    Var::operator bool() const {
        switch (type()) {
            case VarType::Nil:
                return false;
            case VarType::Int:
                return m_data.m_int;
            case VarType::Float:
                return m_data.m_float;
            case VarType::Object:
                return m_data.m_object;
            default:
                return true;
        }
    }
    Var::operator long long() const {
        switch (type()) {
            case VarType::Nil:
                return 0;
            case VarType::Int:
                return m_data.m_int;
            case VarType::Float:
                return static_cast<long long>(m_data.m_float);
            case VarType::String:
                return std::stoll(*as_string());
            case VarType::StringId:
                return std::stoll(as_string_id()->str());
            default:
                return 0;
        }
    }
    Var::operator int() const {
        return static_cast<int>(operator long long());
    }
    Var::operator float() const {
        return static_cast<float>(operator double());
    }
    Var::operator double() const {
        switch (type()) {
            case VarType::Nil:
                return 0;
            case VarType::Int:
                return static_cast<double>(m_data.m_int);
            case VarType::Float:
                return m_data.m_float;
            case VarType::String:
                return std::stod(*as_string());
            case VarType::StringId:
                return std::stod(as_string_id()->str());
            default:
                return 0;
        }
    }
    Var::operator std::size_t() const {
        return static_cast<std::size_t>(operator long long());
    }
    Var::operator String() const {
        switch (type()) {
            case VarType::String:
                return *as_string();
            case VarType::StringId:
                return as_string_id()->str();
            default:
                return String();
        }
    }
    Var::operator StringId() const {
        switch (type()) {
            case VarType::StringId:
                return *as_string_id();
            case VarType::String:
                return StringId(*as_string());
            default:
                return StringId();
        }
    }
    Var::operator Array() const {
        if (type() == VarType::Array)
            return *as_array();
        return Array();
    }
    Var::operator Map() const {
        if (type() == VarType::Map)
            return *as_map();
        return Map();
    }
    Var::operator Ref<Object>() const {
        if (type() == VarType::Object)
            return Ref<Object>(m_data.m_object);
        return Ref<Object>();
    }

    void Var::release() {
        if (m_type != VarType::Nil) {
            switch (m_type) {
                case VarType::String:
                    as_string()->~String();
                    break;
                case VarType::StringId:
                    as_string_id()->~StringId();
                    break;
                case VarType::Array:
                    as_array()->~Array();
                    break;
                case VarType::Map:
                    as_map()->~Map();
                    break;
                case VarType::Object:
                    unref(m_data.m_object);
                    break;
                default:
                    break;
            }

            m_type = VarType::Nil;
        }
    }

    std::string Var::to_string() const {
        std::stringstream stream;
        build_string(stream);
        return stream.str();
    }
    std::size_t Var::hash() const {
        std::size_t accum = 0xffffffff;
        build_hash(accum);
        return accum;
    }

    void Var::build_string(std::stringstream& stream) const {
        switch (type()) {
            case VarType::Nil:
                stream << "nil";
                break;
            case VarType::Int:
                stream << m_data.m_int;
                break;
            case VarType::Float:
                stream << m_data.m_float;
                break;
            case VarType::String:
                stream << '"' << *as_string() << '"';
                break;
            case VarType::StringId:
                stream << *as_string_id();
                break;
            case VarType::Array:
                stream << '[';
                for (const auto& v : *as_array()->get()) {
                    v.build_string(stream);
                    stream << ", ";
                }
                stream << ']';
                break;
            case VarType::Map:
                stream << '{';
                for (const auto& kv : *as_map()->get()) {
                    kv.first.build_string(stream);
                    stream << ": ";
                    kv.second.build_string(stream);
                    stream << ", ";
                }
                stream << '}';
                break;
            case VarType::Object:
                stream << m_data.m_object->to_string();
                break;
            default:
                break;
        }
    }
    void Var::build_hash(std::size_t& accum) const {
        switch (type()) {
            case VarType::Int:
                accum ^= std::hash<long long>()(m_data.m_int);
                break;
            case VarType::Float:
                accum ^= std::hash<double>()(m_data.m_float);
                break;
            case VarType::String:
                accum ^= std::hash<String>()(*as_string());
                break;
            case VarType::StringId:
                accum ^= std::hash<StringId>()(*as_string_id());
                break;
            case VarType::Array:
                for (const auto& v : *as_array()->get()) {
                    v.build_hash(accum);
                }
                break;
            case VarType::Map:
                for (const auto& kv : *as_map()->get()) {
                    kv.first.build_hash(accum);
                    kv.second.build_hash(accum);
                }
            case VarType::Object:
                accum ^= m_data.m_object->hash();
                break;
            default:
                break;
        }
    }

}// namespace wmoge