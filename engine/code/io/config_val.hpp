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

    /** Callback called on config cmd execution */
    using CfgOnCmdExecute = std::function<Status(array_view<std::string> args)>;

    /** Types of supported config objects */
    enum class CfgValType {
        Bool,
        Int,
        Float,
        String,
        Trigger,
        Cmd,
        List
    };

    /**
     * @class CfgValState
     * @brief Shared state of config vals for changes propagation and ownership management
     */
    class CfgValState : public RefCnt {
    public:
        Strid                    name;
        std::string              help;
        CfgValType               type;
        Var                      value;
        Var                      defaul_value;
        std::vector<std::string> options;
        CfgOnCmdExecute          on_execute;
    };

    /**
     * @class CfgValBase
     * @brief Base class for any config object
     */
    class CfgValBase {
    public:
        CfgValBase() = default;
        CfgValBase(Strid name) : m_name(name) {}

        void bind(Ref<CfgValState> state = nullptr) { m_state = std::move(state); }

        [[nodiscard]] const Strid& get_name() const { return m_name; }

    protected:
        Strid            m_name;
        Ref<CfgValState> m_state;
    };

    /**
     * @class CfgVar
     * @brief Cfg val which can hold value of some type
     */
    class CfgVal : public CfgValBase {
    public:
        CfgVal() = default;
        CfgVal(Strid name, Var val = Var()) : CfgValBase(name), m_value(std::move(val)) {}

        [[nodiscard]] const Var& get_value() const { return m_state ? m_state->value : m_value; }
        [[nodiscard]] const Var& get_defaul_value() const { return m_state ? m_state->defaul_value : m_value; }
        [[nodiscard]] VarType    get_value_type() const { return m_state ? m_state->defaul_value.type() : m_value.type(); }

    private:
        Var m_value;
    };

    /**
     * @class CfgCmd
     * @brief Cfg command which can be executed from external
     */
    class CfgCmd : public CfgValBase {
    public:
        [[nodiscard]] const CfgOnCmdExecute& get_on_execute() const { return m_state->on_execute; }
    };

    /**
     * @class CfgTrigger
     * @brief Cfg trigger which can be triggered for one frame
     */
    class CfgTrigger : public CfgVal {
    public:
        CfgTrigger(Strid name) : CfgVal(name, Var(false)) {}

        [[nodiscard]] bool is_triggered() const { return (bool) get_value(); }
    };

    /**
     * @class CfgList
     * @brief Cfg var with list of options to select
     */
    class CfgList : public CfgVal {
    public:
        CfgList(Strid name) : CfgVal(name, Var(int(0))) {}

        [[nodiscard]] const std::vector<std::string>& get_options() const { return m_state->options; }
        [[nodiscard]] int                             get_selected() const { return (int) get_value(); }
    };

    /**
     * @class CfgValT
     * @brief Typed wrapper for a config val
     */
    template<typename T>
    class CfgValT : public CfgVal {
    public:
        CfgValT() = default;
        CfgValT(Strid name, T val = T()) : CfgVal(name, Var(val)) {}

        [[nodiscard]] T get_value_of() const { return (T) get_value(); }
        [[nodiscard]] T get_default_value_of() const { return (T) get_defaul_value(); }

        operator T() const { return get_value_of(); }
    };

    using CfgValBool   = CfgValT<bool>;
    using CfgValInt    = CfgValT<int>;
    using CfgValFloat  = CfgValT<float>;
    using CfgValString = CfgValT<std::string>;

}// namespace wmoge