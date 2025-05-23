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

#include "gfx/gfx_cmd_list.hpp"
#include "rdg/rdg_graph.hpp"
#include "ui/ui_style.hpp"
#include "ui/ui_window.hpp"

namespace wmoge {

    /**
     * @class UiManager
     * @brief Interface for ui sub-system
     */
    class UiManager {
    public:
        virtual ~UiManager() = default;

        virtual void                set_main_window(Ref<UiMainWindow> window)   = 0;
        virtual void                add_dock_window(Ref<UiDockWindow> window)   = 0;
        virtual void                update(std::size_t frame_id)                = 0;
        virtual void                render(RdgGraph& graph, RdgTexture* target) = 0;
        virtual void                set_style(const Ref<UiStyle>& style)        = 0;
        virtual const Ref<UiStyle>& get_style()                                 = 0;
        virtual const Ref<UiStyle>& get_style_default()                         = 0;
    };

}// namespace wmoge