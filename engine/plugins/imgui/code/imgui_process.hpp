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
#include "grc/icon.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_style.hpp"

namespace wmoge {

    class ImguiProcessor {
    public:
        ImguiProcessor(class ImguiManager* manager);

        void             process_tree(UiElement* element);
        void             process(UiElement* element);
        void             process(std::vector<Ref<UiElement>>& elements);
        void             draw_icon(const Icon& icon, const Vec2f& icon_size);
        void             add_action_event(std::function<void()>& event);
        void             add_action(std::function<void()> action);
        void             dispatch_actions();
        void             clear_actions();
        array_view<char> put_str_to_buffer(const std::string& s);
        std::string      pop_str_from_buffer();
        Color4f          get_color(UiColor color) const;

        [[nodiscard]] class ImguiManager* get_manager() const { return m_manager; }

    private:
        void push_style();
        void pop_style();
        void push_sub_style(Strid sub_style);
        void pop_sub_style();
        void push_param(UiParam param, float value);
        void pop_param(UiParam param);
        void push_color(UiColor color, Color4f value);
        void pop_color(UiColor color);

    private:
        std::vector<std::function<void()>> m_actions;
        std::vector<char>                  m_input_buffer;
        std::vector<std::vector<float>>    m_param_stack;
        std::vector<std::vector<Color4f>>  m_color_stack;
        std::vector<const UiSubStyle*>     m_style_stack;
        class ImguiManager*                m_manager;
    };

}// namespace wmoge