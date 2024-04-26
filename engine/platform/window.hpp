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

#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "math/vec.hpp"
#include "render/image.hpp"

namespace wmoge {

    /**
     * @class WindowInfo
     * @brief Struct holding window creation info
     */
    struct WindowInfo {
        int         width  = 1280;
        int         height = 720;
        Strid       id     = SID("primary");
        std::string title  = "Window";
        Ref<Image>  icons[2];
    };

    /**
     * @class Window
     * @brief Interface for OS-specific window for displaying graphics
     */
    class Window : public RefCnt {
    public:
        ~Window() override                            = default;
        virtual void               close()            = 0;
        virtual int                width() const      = 0;
        virtual int                height() const     = 0;
        virtual Size2i             size() const       = 0;
        virtual int                fbo_width() const  = 0;
        virtual int                fbo_height() const = 0;
        virtual Size2i             fbo_size() const   = 0;
        virtual float              scale_x() const    = 0;
        virtual float              scale_y() const    = 0;
        virtual bool               in_focus() const   = 0;
        virtual const Strid&       id() const         = 0;
        virtual const std::string& title() const      = 0;
    };

}// namespace wmoge