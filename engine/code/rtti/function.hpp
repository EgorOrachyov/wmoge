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
#include "core/flat_map.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "rtti/meta_data.hpp"
#include "rtti/type.hpp"

#include <functional>
#include <vector>

namespace wmoge {

    /**
     * @class RttiParamInfo
     * @brief Info of a callable function param
    */
    struct RttiParamInfo {
        RttiType*   type = nullptr;
        Strid       name;
        bool        is_const     = false;
        bool        is_ref       = false;
        bool        is_ptr       = false;
        std::size_t stack_size   = 0;
        std::size_t stack_offset = 0;

        void print_param(std::stringstream& function_name) const;
    };

    /**
     * @class RttiFrame
     * @brief Context of rtti function call
    */
    struct RttiFrame {};

    /**
     * @brief Something callable through rtti system
    */
    using RttiCallable = std::function<Status(RttiFrame& frame, void* p_args_ret)>;

    /**
     * @class RttiTypeFunction
     * @brief Base rtti to inspect function like types
    */
    class RttiTypeFunction : public RttiType {
    public:
        RttiTypeFunction(Strid name, std::size_t byte_size, std::size_t stack_size, std::vector<RttiParamInfo> args, RttiParamInfo ret)
            : RttiType(name, byte_size, RttiArchetype::Function),
              m_stack_size(stack_size),
              m_args(std::move(args)),
              m_ret(std::move(ret)) {
            m_signature = make_signature(m_args, m_ret);
        }

        virtual Status call(RttiFrame& frame, void* target, array_view<std::uint8_t> args_ret) const { return StatusCode::NotImplemented; }

        [[nodiscard]] const Strid&                      get_name() const { return m_name; }
        [[nodiscard]] const Strid&                      get_signature() const { return m_signature; }
        [[nodiscard]] std::size_t                       get_byte_size() const { return m_byte_size; }
        [[nodiscard]] std::size_t                       get_stack_size() const { return m_stack_size; }
        [[nodiscard]] const std::vector<RttiParamInfo>& get_args() const { return m_args; }
        [[nodiscard]] const RttiParamInfo&              get_ret() const { return m_ret; }
        [[nodiscard]] bool                              has_ret() const { return m_ret.type; }

        static Strid make_signature(array_view<const RttiParamInfo> args, const RttiParamInfo& ret);

    protected:
        std::vector<RttiParamInfo> m_args;
        RttiParamInfo              m_ret;
        std::size_t                m_byte_size;
        std::size_t                m_stack_size;
        Strid                      m_name;
        Strid                      m_signature;
    };

    /**
     * @class RttiFunctionSignal
     * @brief Function callable with signal interface
     * 
    */
    class RttiFunctionSignal : public RttiTypeFunction {
    public:
        RttiFunctionSignal(Strid name, std::size_t byte_size, std::size_t stack_size, std::vector<RttiParamInfo> args, RttiParamInfo ret) : RttiTypeFunction(name, byte_size, stack_size, std::move(args), std::move(ret)) {}
        ~RttiFunctionSignal() override = default;

        virtual Status bind(RttiCallable callable, int& id) { return StatusCode::NotImplemented; }
        virtual Status unbind(int id) { return StatusCode::NotImplemented; }
    };

}// namespace wmoge