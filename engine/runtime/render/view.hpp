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
#include "gfx/gfx_texture.hpp"
#include "math/color.hpp"
#include "math/vec.hpp"
#include "platform/window.hpp"
#include "render/camera.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class ViewParams
     * @brief Holds params assositated to the view
    */
    struct ViewParams {
        bool auto_aspect = true;
    };

    /**
     * @class View
     * @brief View control how to present rendered content to the screen
    */
    class View : public RefCnt {
    public:
        View(Strid name);
        ~View() override = default;

        void update(bool is_active);

        [[nodiscard]] const ViewParams&            get_params() const { return m_params; }
        [[nodiscard]] const Camera&                get_camera() const { return m_camera; }
        [[nodiscard]] const std::optional<Camera>& get_camera_prev() const { return m_camera_prev; }
        [[nodiscard]] const Strid&                 get_name() const { return m_name; }

    private:
        ViewParams            m_params;
        Camera                m_camera;
        std::optional<Camera> m_camera_prev;
        Strid                 m_name;
        Ref<Window>           m_window;
        Rect2i                m_presentation_area  = Rect2i(0, 0, 1280, 720);
        Color4f               m_presentation_color = Color::BLACK4f;
    };

}// namespace wmoge