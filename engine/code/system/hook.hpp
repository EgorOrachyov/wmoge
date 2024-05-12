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

#include "core/cmd_line.hpp"
#include "core/status.hpp"

#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class Hook
     * @brief Interface for an engine hook to perform some actions on lower engine levels
     */
    class Hook {
    public:
        virtual ~Hook() = default;

        virtual std::string get_name() const { return ""; }
        virtual void        on_attach() {}
        virtual void        on_add_cmd_line_options(CmdLine& cmd_line) {}
        virtual Status      on_process(CmdLine& cmd_line) { return WG_OK; }
    };

    /** 
     * @class HookList
     * @brief Storage for application hooks
     */
    class HookList {
    public:
        HookList()                = default;
        HookList(const HookList&) = delete;
        HookList(HookList&&)      = delete;
        ~HookList()               = default;

        using HookPtr     = std::shared_ptr<Hook>;
        using HookStorage = std::vector<HookPtr>;

        template<typename F>
        void each(F&& f);

        void attach(HookPtr hook);
        void remove(HookPtr hook);
        void clear();

    private:
        HookStorage m_storage;
    };

    template<typename F>
    inline void HookList::each(F&& f) {
        for (auto& hook : m_storage) {
            if (f(hook)) { break; }
        }
    }

}// namespace wmoge