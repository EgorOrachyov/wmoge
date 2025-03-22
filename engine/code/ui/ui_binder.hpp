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
#include "core/weak_ref.hpp"
#include "ui/ui_bindable.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_markup.hpp"

namespace wmoge {

    /**
     * @class UiBindMediator
     * @brief Holds information about bound ui elements and bindable code
     */
    class UiBindMediator : public WeakRefCnt<UiBindMediator, RefCnt> {
    public:
        flat_map<Strid, UiElement*>            tagged_elements;
        flat_map<Strid, std::function<void()>> binded_properties;
        UiElement*                             root_element = nullptr;
        Ref<UiBindable>                        bindable;
    };

    /**
     * @class UiBinder
     * @brief Builds markup and bindable instance into a ui element
     */
    class UiBinder {
    public:
        UiBinder(Ref<UiElement>& element, const Ref<UiMarkup>& markup, const Ref<UiBindable>& bindalbe);

        [[nodiscard]] Status bind();

    private:
        [[nodiscard]] Status bind_element(Ref<UiElement>& element, int element_id);
        [[nodiscard]] Status bind_element_slot(const Ref<UiElement>& element, int slot_id);
        [[nodiscard]] Status bind_element_attribute(const Ref<UiElement>& element, int attribute_id);

    private:
        Ref<UiElement>&     m_element;
        Ref<UiMarkup>       m_markup;
        Ref<UiBindable>     m_bindalbe;
        Ref<UiBindMediator> m_mediator;
    };

}// namespace wmoge