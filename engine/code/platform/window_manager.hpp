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

#include "core/buffered_vector.hpp"
#include "platform/window.hpp"

#include <vector>

namespace wmoge {

    /** @brief Type of window manager */
    enum class WindowManagerType {
        None = 0,
        Glfw = 1
    };

    /** @brief Type of window changes */
    enum class WindowNotification {
        Minimized,
        Maximized,
        Restored,
        Resized,
        FocusReceived,
        FocusLost,
        FramebufferResized,
        ContentScale,
        CloseRequested
    };

    /**
     * @class WindowEvent
     * @brief Platform window event
     */
    struct WindowEvent {
        Ref<Window>        window;
        WindowNotification notification;
    };

    /**
     * @class WindowManager
     * @brief OS-specific manager for windows
     */
    class WindowManager {
    public:
        virtual ~WindowManager()                                                             = default;
        virtual void                            poll_events()                                = 0;
        virtual buffered_vector<Ref<Window>>    get_windows()                                = 0;
        virtual Ref<Window>                     get_primary_window()                         = 0;
        virtual Ref<Window>                     create_window(const WindowInfo& window_info) = 0;
        virtual Ref<Window>                     get_window(const Strid& window_id)           = 0;
        virtual const std::vector<WindowEvent>& get_window_events()                          = 0;
        virtual WindowManagerType               get_type()                                   = 0;
    };

}// namespace wmoge