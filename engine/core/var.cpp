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

#include "core/crc32.hpp"
#include "core/object.hpp"

#include <algorithm>
#include <utility>

namespace wmoge {

    std::size_t VarHash::operator()(const class Var& v) const {
        return v.hash();
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
    Var::Var(Strid value) {
        m_type = VarType::Strid;
        new (m_data.m_mem) Strid(std::move(value));
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
    Var::Var(std::vector<Var> value) : Var(Array(std::move(value))) {
    }
    Var::Var(std::unordered_map<Var, Var, VarHash> value) : Var(Map(std::move(value))) {
    }
    Var::Var(ArrayByte value) {
        m_type = VarType::ArrayByte;
        new (m_data.m_mem) ArrayByte(std::move(value));
    }
    Var::Var(ArrayInt value) {
        m_type = VarType::ArrayInt;
        new (m_data.m_mem) ArrayInt(std::move(value));
    }
    Var::Var(ArrayFloat value) {
        m_type = VarType::ArrayFloat;
        new (m_data.m_mem) ArrayFloat(std::move(value));
    }
    Var::Var(ArrayString value) {
        m_type = VarType::ArrayString;
        new (m_data.m_mem) ArrayString(std::move(value));
    }
    Var::Var(ArrayVec2f value) {
        m_type = VarType::ArrayVec2f;
        new (m_data.m_mem) ArrayVec2f(std::move(value));
    }
    Var::Var(ArrayVec3f value) {
        m_type = VarType::ArrayVec3f;
        new (m_data.m_mem) ArrayVec3f(std::move(value));
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
                new (m_data.m_mem) String(var.as<String>());
                break;
            case VarType::Strid:
                new (m_data.m_mem) Strid(var.as<Strid>());
                break;
            case VarType::Array:
                new (m_data.m_mem) Array(var.as<Array>());
                break;
            case VarType::Map:
                new (m_data.m_mem) Map(var.as<Map>());
                break;
            case VarType::Object:
                m_data.m_object = ref(var.m_data.m_object);
                break;
            case VarType::ArrayByte:
                new (m_data.m_mem) ArrayByte(var.as<ArrayByte>());
                break;
            case VarType::ArrayInt:
                new (m_data.m_mem) ArrayInt(var.as<ArrayInt>());
                break;
            case VarType::ArrayFloat:
                new (m_data.m_mem) ArrayFloat(var.as<ArrayFloat>());
                break;
            case VarType::ArrayString:
                new (m_data.m_mem) ArrayString(var.as<ArrayString>());
                break;
            case VarType::ArrayVec2f:
                new (m_data.m_mem) ArrayVec2f(var.as<ArrayVec2f>());
                break;
            case VarType::ArrayVec3f:
                new (m_data.m_mem) ArrayVec3f(var.as<ArrayVec3f>());
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
                new (m_data.m_mem) String(std::move(var.as<String>()));
                break;
            case VarType::Strid:
                new (m_data.m_mem) Strid(std::move(var.as<Strid>()));
                break;
            case VarType::Array:
                new (m_data.m_mem) Array(std::move(var.as<Array>()));
                break;
            case VarType::Map:
                new (m_data.m_mem) Map(std::move(var.as<Map>()));
                break;
            case VarType::Object:
                m_data.m_object = ref(var.m_data.m_object);
                break;
            case VarType::ArrayByte:
                new (m_data.m_mem) ArrayByte(std::move(var.as<ArrayByte>()));
                break;
            case VarType::ArrayInt:
                new (m_data.m_mem) ArrayInt(std::move(var.as<ArrayInt>()));
                break;
            case VarType::ArrayFloat:
                new (m_data.m_mem) ArrayFloat(std::move(var.as<ArrayFloat>()));
                break;
            case VarType::ArrayString:
                new (m_data.m_mem) ArrayString(std::move(var.as<ArrayString>()));
                break;
            case VarType::ArrayVec2f:
                new (m_data.m_mem) ArrayVec2f(std::move(var.as<ArrayVec2f>()));
                break;
            case VarType::ArrayVec3f:
                new (m_data.m_mem) ArrayVec3f(std::move(var.as<ArrayVec3f>()));
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
                return as<String>() == var.as<String>();
            case VarType::Strid:
                return as<Strid>() == var.as<Strid>();
            case VarType::Array: {
                const auto& ar1 = as<Array>();
                const auto& ar2 = var.as<Array>();
                return (ar1.size() == ar2.size()) && std::equal(ar1.begin(), ar1.end(), ar2.begin());
            }
            case VarType::Map: {
                const auto& mp1 = as<Map>();
                const auto& mp2 = var.as<Map>();
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
                return as<String>() != var.as<String>();
            case VarType::Strid:
                return as<Strid>() != var.as<Strid>();
            case VarType::Array: {
                const auto& ar1 = as<Array>();
                const auto& ar2 = var.as<Array>();
                return (ar1.size() != ar2.size()) || !std::equal(ar1.begin(), ar1.end(), ar2.begin());
            }
            case VarType::Map: {
                const auto& mp1 = as<Map>();
                const auto& mp2 = var.as<Map>();
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
                return as<String>() < var.as<String>();
            case VarType::Strid:
                return as<Strid>() < var.as<Strid>();
            case VarType::Array: {
                const auto& ar1 = as<Array>();
                const auto& ar2 = var.as<Array>();
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
                const auto& mp1 = as<Map>();
                const auto& mp2 = var.as<Map>();
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
                return false;
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
                return std::stoll(as<String>());
            case VarType::Strid:
                return std::stoll(as<Strid>().str());
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
                return std::stod(as<String>());
            case VarType::Strid:
                return std::stod(as<Strid>().str());
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
                return as<String>();
            case VarType::Strid:
                return as<Strid>().str();
            default:
                return String();
        }
    }
    Var::operator Strid() const {
        switch (type()) {
            case VarType::Strid:
                return as<Strid>();
            case VarType::String:
                return Strid(as<String>());
            default:
                return Strid();
        }
    }
    Var::operator Array() const {
        if (type() == VarType::Array)
            return as<Array>();
        return Array();
    }
    Var::operator Map() const {
        if (type() == VarType::Map)
            return as<Map>();
        return Map();
    }
    Var::operator Ref<Object>() const {
        if (type() == VarType::Object)
            return Ref<Object>(m_data.m_object);
        return Ref<Object>();
    }
    Var::operator ArrayByte() const {
        if (type() == VarType::ArrayByte)
            return as<ArrayByte>();
        return ArrayByte();
    }
    Var::operator ArrayInt() const {
        if (type() == VarType::ArrayInt)
            return as<ArrayInt>();
        return ArrayInt();
    }
    Var::operator ArrayFloat() const {
        if (type() == VarType::ArrayFloat)
            return as<ArrayFloat>();
        return ArrayFloat();
    }
    Var::operator ArrayString() const {
        if (type() == VarType::ArrayString)
            return as<ArrayString>();
        return ArrayString();
    }
    Var::operator ArrayVec2f() const {
        if (type() == VarType::ArrayVec2f)
            return as<ArrayVec2f>();
        return ArrayVec2f();
    }
    Var::operator ArrayVec3f() const {
        if (type() == VarType::ArrayVec3f)
            return as<ArrayVec3f>();
        return ArrayVec3f();
    }

    void Var::release() {
        if (m_type != VarType::Nil) {
            switch (m_type) {
                case VarType::String:
                    as<String>().~String();
                    break;
                case VarType::Strid:
                    as<Strid>().~Strid();
                    break;
                case VarType::Array:
                    as<Array>().~Array();
                    break;
                case VarType::Map:
                    as<Map>().~Map();
                    break;
                case VarType::Object:
                    unref(m_data.m_object);
                    break;
                case VarType::ArrayByte:
                    as<ArrayByte>().~ArrayByte();
                    break;
                case VarType::ArrayInt:
                    as<ArrayInt>().~ArrayInt();
                    break;
                case VarType::ArrayFloat:
                    as<ArrayFloat>().~ArrayFloat();
                    break;
                case VarType::ArrayString:
                    as<ArrayString>().~ArrayString();
                    break;
                case VarType::ArrayVec2f:
                    as<ArrayVec2f>().~ArrayVec2f();
                    break;
                case VarType::ArrayVec3f:
                    as<ArrayVec3f>().~ArrayVec3f();
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
                stream << '"' << as<String>() << '"';
                break;
            case VarType::Strid:
                stream << as<Strid>();
                break;
            case VarType::Array:
                stream << '[';
                for (const auto& v : as<Array>()) {
                    v.build_string(stream);
                    stream << ", ";
                }
                stream << ']';
                break;
            case VarType::Map:
                stream << '{';
                for (const auto& kv : as<Map>()) {
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
            case VarType::ArrayByte:
                stream << '[';
                for (const auto& v : as<ArrayByte>()) {
                    stream << v;
                    stream << ", ";
                }
                stream << ']';
                break;
            case VarType::ArrayInt:
                stream << '[';
                for (const auto& v : as<ArrayInt>()) {
                    stream << v;
                    stream << ", ";
                }
                stream << ']';
                break;
            case VarType::ArrayFloat:
                stream << '[';
                for (const auto& v : as<ArrayFloat>()) {
                    stream << v;
                    stream << ", ";
                }
                stream << ']';
                break;
            case VarType::ArrayString:
                stream << '[';
                for (const auto& v : as<ArrayString>()) {
                    stream << '\"' << v << '\"';
                    stream << ", ";
                }
                stream << ']';
                break;
            case VarType::ArrayVec2f:
                stream << '[';
                for (const auto& v : as<ArrayVec2f>()) {
                    stream << v;
                    stream << ", ";
                }
                stream << ']';
                break;
            case VarType::ArrayVec3f:
                stream << '[';
                for (const auto& v : as<ArrayVec3f>()) {
                    stream << v;
                    stream << ", ";
                }
                stream << ']';
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
                accum ^= std::hash<String>()(as<String>());
                break;
            case VarType::Strid:
                accum ^= std::hash<Strid>()(as<Strid>());
                break;
            case VarType::Array:
                for (const auto& v : as<Array>()) {
                    v.build_hash(accum);
                }
                break;
            case VarType::Map:
                for (const auto& kv : as<Map>()) {
                    kv.first.build_hash(accum);
                    kv.second.build_hash(accum);
                }
            case VarType::Object:
                accum ^= m_data.m_object->hash();
                break;
            case VarType::ArrayByte:
                accum ^= Crc32Util::hash(as<ArrayByte>().data(), as<ArrayByte>().size());
                break;
            case VarType::ArrayInt:
                accum ^= Crc32Util::hash(as<ArrayInt>().data(), as<ArrayInt>().size());
                break;
            case VarType::ArrayFloat:
                accum ^= Crc32Util::hash(as<ArrayFloat>().data(), as<ArrayFloat>().size());
                break;
            case VarType::ArrayString:
                for (const auto& v : as<ArrayString>()) {
                    accum ^= std::hash<String>()(v);
                }
                break;
            case VarType::ArrayVec2f:
                accum ^= Crc32Util::hash(as<ArrayVec2f>().data(), as<ArrayVec2f>().size() * sizeof(Vec2f));
                break;
            case VarType::ArrayVec3f:
                accum ^= Crc32Util::hash(as<ArrayVec3f>().data(), as<ArrayVec3f>().size() * sizeof(Vec3f));
                break;
            default:
                break;
        }
    }

}// namespace wmoge