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
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "rtti/traits.hpp"

#include <functional>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class ConsoleObject
     * @brief Base class for any console object
     */
    class ConsoleObject : public RttiObject {
    public:
        WG_RTTI_CLASS(ConsoleObject, RttiObject)

        ConsoleObject() = default;

        ConsoleObject(Strid name, std::string help)
            : m_name(name), m_help(std::move(help)) {
        }

        [[nodiscard]] const Strid&       get_name() const { return m_name; }
        [[nodiscard]] const std::string& get_help() const { return m_help; }

    protected:
        friend class ConsoleManager;

        Strid       m_name;
        std::string m_help;
    };

    WG_RTTI_CLASS_BEGIN(ConsoleObject) {
        WG_RTTI_FIELD(m_name, {});
        WG_RTTI_FIELD(m_help, {});
    }
    WG_RTTI_END;

    /**
     * @class ConsoleVar
     * @brief Console var which can hold some value
     */
    class ConsoleVar : public ConsoleObject {
    public:
        WG_RTTI_CLASS(ConsoleVar, ConsoleObject)

        ConsoleVar() = default;

        ConsoleVar(Strid name, std::string help, Var value)
            : ConsoleObject(name, std::move(help)),
              m_value(value),
              m_defaul_value(value) {
        }

        [[nodiscard]] const Var& get_value() const { return m_value; }
        [[nodiscard]] const Var& get_defaul_value() const { return m_defaul_value; }
        [[nodiscard]] VarType    get_value_type() const { return m_defaul_value.type(); }

    protected:
        friend class ConsoleManager;

        Var m_value;
        Var m_defaul_value;
    };

    WG_RTTI_CLASS_BEGIN(ConsoleVar) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_value, {});
        WG_RTTI_FIELD(m_defaul_value, {});
    }
    WG_RTTI_END;

    /**
     * @class ConsoleCmd
     * @brief Console command which can be executed from console
     */
    class ConsoleCmd : public ConsoleObject {
    public:
        WG_RTTI_CLASS(ConsoleCmd, ConsoleObject)

        using OnExecute = std::function<Status(array_view<std::string> args)>;

        ConsoleCmd() = default;

        ConsoleCmd(Strid name, std::string help, OnExecute on_execute)
            : ConsoleObject(name, std::move(help)),
              m_on_execute(std::move(on_execute)) {
        }

        [[nodiscard]] const OnExecute& get_on_execute() const { return m_on_execute; }

    protected:
        friend class ConsoleManager;

        OnExecute m_on_execute;
    };

    WG_RTTI_CLASS_BEGIN(ConsoleCmd) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class ConsoleTrigger
     * @brief Console trigger which can be triggered for one frame
     */
    class ConsoleTrigger : public ConsoleObject {
    public:
        WG_RTTI_CLASS(ConsoleTrigger, ConsoleObject)

        ConsoleTrigger() = default;

        ConsoleTrigger(Strid name, std::string help)
            : ConsoleObject(name, std::move(help)) {
        }

        [[nodiscard]] bool is_triggered() const { return m_triggered; }

    protected:
        friend class ConsoleManager;

        bool m_triggered = false;
    };

    WG_RTTI_CLASS_BEGIN(ConsoleTrigger) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_triggered, {});
    }
    WG_RTTI_END;

}// namespace wmoge