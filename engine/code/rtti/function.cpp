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

#include "function.hpp"

namespace wmoge {

    void RttiParamInfo::print_param(std::stringstream& function_name) const {
        if (!type) {
            function_name << "void";
            return;
        }
        if (is_const) {
            function_name << "const ";
        }

        function_name << type->get_str();

        if (is_ptr) {
            function_name << "*";
        }
        if (is_ref) {
            function_name << "&";
        }
        if (!name.empty()) {
            function_name << " ";
            function_name << name.str();
        }
    }

    Strid RttiTypeFunction::make_signature(array_view<const RttiParamInfo> args, const RttiParamInfo& ret) {
        std::stringstream signature;

        ret.print_param(signature);
        signature << "(";

        for (const auto& arg : args) {
            arg.print_param(signature);
            signature << ",";
        }

        signature << ")";

        return Strid(signature.str());
    }

}// namespace wmoge