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

#ifndef WMOGE_CANVAS_TEXT_HPP
#define WMOGE_CANVAS_TEXT_HPP

#include "components/canvas_item.hpp"
#include "resource/font.hpp"

namespace wmoge {

    /**
     * @class CanvasText
     * @brief Displays text in a line on canvas
     *
     * Allows to render a text label on 2d screen with selected font and color.
     * Font size can be adjusted. Optional text alignment may be used to
     * automatically center font.
     */
    class CanvasText : public CanvasItem {
    public:
        WG_OBJECT(CanvasText, CanvasItem)

        void                 set_text(std::string text);
        void                 set_font(ref_ptr<Font> font);
        void                 set_font_size(float font_size);
        void                 set_centered();
        const std::string&   get_text();
        const ref_ptr<Font>& get_font();
        float                get_font_size();

        bool on_load_from_yaml(const YamlConstNodeRef& node) override;
        void on_scene_enter() override;
        void on_scene_exit() override;

    private:
        std::string       m_text;
        ref_ptr<Font>     m_font;
        float             m_font_size = 1.0f;
        FontTextAlignment m_alignment = FontTextAlignment::Left;
    };

}// namespace wmoge

#endif//WMOGE_CANVAS_TEXT_HPP
