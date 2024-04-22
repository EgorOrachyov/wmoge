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

#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "rtti/function.hpp"
#include "rtti/meta_data.hpp"
#include "rtti/struct.hpp"
#include "rtti/type.hpp"

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class RttiMethod
     * @brief Represents a class method
    */
    class RttiMethod : public RttiMember {
    public:
        RttiMethod(Strid name, Ref<RttiFunction> function) : RttiMember(name) {
            m_function = std::move(function);
        }

        [[nodiscard]] const Ref<RttiFunction>& get_function() const { return m_function; }

    private:
        Ref<RttiFunction> m_function;
    };

    /**
     * @class RttiSignal
     * @brief Represents a class signal
    */
    class RttiSignal : public RttiMember {
    public:
        RttiSignal(Strid name, Ref<RttiFunctionSignal> function) : RttiMember(name) {
            m_function = std::move(function);
        }

        [[nodiscard]] const Ref<RttiFunctionSignal>& get_function() const { return m_function; }

    private:
        Ref<RttiFunctionSignal> m_function;
    };

    /**
     * @class RttiClass
     * @brief Class refletion info for all types inherited from engine Object type
    */
    class RttiClass : public RttiStruct {
    public:
        RttiClass(Strid name, std::size_t byte_size, RttiClass* parent);
        ~RttiClass() override = default;

        std::optional<const RttiMethod*> find_method(const Strid& name) const;
        void                             add_method(RttiMethod method);
        bool                             has_method(const Strid& name) const;
        std::optional<const RttiSignal*> find_signal(const Strid& name) const;
        void                             add_signal(RttiSignal signal);
        bool                             has_signal(const Strid& name) const;
        void                             add_factory(std::function<class RttiObject*()> factory);
        bool                             can_instantiate() const;
        Ref<class RttiObject>            instantiate() const;

        [[nodiscard]] const std::function<class RttiObject*()>& get_factory() const { return m_factory; }
        [[nodiscard]] const flat_map<Strid, int>&               get_methods_map() const { return m_methods_map; }
        [[nodiscard]] const std::vector<RttiMethod>&            get_methods() const { return m_methods; }
        [[nodiscard]] const flat_map<Strid, int>&               get_signals_map() const { return m_signals_map; }
        [[nodiscard]] const std::vector<RttiSignal>&            get_signals() const { return m_signals; }
        [[nodiscard]] RttiClass*                                get_parent_class() const { return m_parent_class; }

    private:
        std::function<class RttiObject*()> m_factory;
        flat_map<Strid, int>               m_methods_map;
        std::vector<RttiMethod>            m_methods;
        flat_map<Strid, int>               m_signals_map;
        std::vector<RttiSignal>            m_signals;
        RttiClass*                         m_parent_class = nullptr;
    };

}// namespace wmoge