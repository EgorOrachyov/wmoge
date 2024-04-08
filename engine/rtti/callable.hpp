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
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "rtti/meta_data.hpp"
#include "rtti/type.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class RttiParamInfo
     * @brief Info of a callable function param
    */
    struct RttiParamInfo {
        RttiType* type;
        Strid     name;
        bool      is_const = false;
        bool      is_ref   = false;
        bool      is_ptr   = false;
    };

    /**
     * @class RttiCallable
     * @brief Represents type of something callable with function signature
    */
    class RttiCallable : public RttiType {
    public:
        RttiCallable(Strid name, std::size_t byte_size) : RttiType(name, byte_size) {}
        ~RttiCallable() override = default;

        void add_arg(RttiParamInfo arg);
        void add_ret(RttiParamInfo ret);

        [[nodiscard]] const std::vector<RttiParamInfo>& get_args() const { return m_args; }
        [[nodiscard]] const RttiParamInfo&              get_ret() const { return m_ret; }
        [[nodiscard]] bool                              has_ret() const { return m_ret.type; }

    private:
        std::vector<RttiParamInfo> m_args;
        RttiParamInfo              m_ret;
    };

}// namespace wmoge