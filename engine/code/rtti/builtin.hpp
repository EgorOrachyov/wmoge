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

#include "core/array_view.hpp"
#include "rtti/type.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class RttiTypeVec
     * @brief Base rtti to inspect Vec values
    */
    class RttiTypeVec : public RttiType {
    public:
        RttiTypeVec(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        [[nodiscard]] const RttiType* get_value_type() const { return m_value_type; }
        [[nodiscard]] int             get_dimension() const { return m_dimension; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Vec; }

    protected:
        RttiType* m_value_type = nullptr;
        int       m_dimension  = 0;
    };

    /**
     * @class RttiTypeMask
     * @brief Base rtti to inspect bit mask
    */
    class RttiTypeMask : public RttiType {
    public:
        RttiTypeMask(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        [[nodiscard]] const RttiType* get_value_type() const { return m_value_type; }
        [[nodiscard]] int             get_dimension() const { return m_dimension; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Mask; }

    protected:
        RttiType* m_value_type = nullptr;
        int       m_dimension  = 0;
    };

    /**
     * @class RttiTypeBitset
     * @brief Base rtti to inspect bit set
    */
    class RttiTypeBitset : public RttiType {
    public:
        RttiTypeBitset(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        [[nodiscard]] int get_dimension() const { return m_dimension; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Bitset; }

    protected:
        int m_dimension = 0;
    };

    /**
     * @class RttiTypeRef
     * @brief Base rtti to inspect ref counted value
    */
    class RttiTypeRef : public RttiType {
    public:
        RttiTypeRef(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        [[nodiscard]] const RttiType* get_value_type() const { return m_value_type; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Ref; }

    protected:
        RttiType* m_value_type = nullptr;
    };

    /**
     * @class RttiTypeOptional
     * @brief Base rtti to inspect optional value
    */
    class RttiTypeOptional : public RttiType {
    public:
        RttiTypeOptional(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        virtual Status set_value(void* dst, const void* src) const { return StatusCode::NotImplemented; }

        [[nodiscard]] const RttiType* get_value_type() const { return m_value_type; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Optional; }

    protected:
        RttiType* m_value_type = nullptr;
    };

    /**
     * @class RttiTypeVector
     * @brief Base rtti to inspect vector containers
    */
    class RttiTypeVector : public RttiType {
    public:
        RttiTypeVector(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        virtual Status push_back(void* src, void* value) const { return StatusCode::NotImplemented; }

        [[nodiscard]] const RttiType* get_value_type() const { return m_value_type; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Vector; }

    protected:
        RttiType* m_value_type = nullptr;
    };

    /**
     * @class RttiTypeSet
     * @brief Base rtti to inspect set containers
    */
    class RttiTypeSet : public RttiType {
    public:
        RttiTypeSet(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        [[nodiscard]] const RttiType* get_value_type() const { return m_value_type; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Set; }

    protected:
        RttiType* m_value_type = nullptr;
    };

    /**
     * @class RttiTypeMap
     * @brief Base rtti to inspect map containers
    */
    class RttiTypeMap : public RttiType {
    public:
        RttiTypeMap(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        [[nodiscard]] const RttiType* get_key_type() const { return m_key_type; }
        [[nodiscard]] const RttiType* get_value_type() const { return m_value_type; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Map; }

    protected:
        RttiType* m_key_type   = nullptr;
        RttiType* m_value_type = nullptr;
    };

    /**
     * @class RttiTypePair
     * @brief Base rtti to inspect pair containers
    */
    class RttiTypePair : public RttiType {
    public:
        RttiTypePair(Strid name, std::size_t byte_size) : RttiType(name, byte_size, static_archetype()) {}

        [[nodiscard]] const RttiType* get_key_type() const { return m_key_type; }
        [[nodiscard]] const RttiType* get_value_type() const { return m_value_type; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Pair; }

    protected:
        RttiType* m_key_type   = nullptr;
        RttiType* m_value_type = nullptr;
    };

}// namespace wmoge