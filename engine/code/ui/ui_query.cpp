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

#include "ui_query.hpp"

#include "ui/ui_inputs.hpp"
#include "ui/ui_tooltip.hpp"
#include "ui/ui_window.hpp"

namespace wmoge {

    UiQuery& UiQuery::tag(Strid tag) {
        m_tag = tag;
        return *this;
    }

    UiQuery& UiQuery::cls(RttiClass* cls) {
        m_cls = cls;
        return *this;
    }

    UiQuery& UiQuery::multiple() {
        m_multiple = true;
        return *this;
    }

    Status UiQuery::exec(const Ref<UiElement>& root) {
        return exec(root.get());
    }

    Status UiQuery::exec(UiElement* element) {
        if (!element) {
            return WG_OK;
        }

        if (!m_multiple && !m_results.empty()) {
            return WG_OK;
        }

        bool mathed_tag = true;
        bool mathed_cls = true;

        if (m_tag) {
            mathed_tag = element->tag == *m_tag;
        }
        if (m_cls) {
            mathed_cls = element->get_class() == *m_cls;
        }

        if (mathed_tag && mathed_cls) {
            m_results.push_back(Ref<UiElement>(element));
        }

        switch (element->type) {
            case UiElementType::CollapsingPanel: {
                auto e = static_cast<UiCollapsingPanel*>(element);
                WG_CHECKED(exec(e->header.get()));
            } break;

            case UiElementType::InputTextExt: {
                auto e = static_cast<UiInputTextExt*>(element);
                WG_CHECKED(exec(e->completion_popup.get()));
            } break;

            case UiElementType::MainWindow:
            case UiElementType::DockWindow: {
                auto e = static_cast<UiWindow*>(element);
                WG_CHECKED(exec(e->menu_bar.get()));
                WG_CHECKED(exec(e->tool_bar.get()));
                WG_CHECKED(exec(e->status_bar.get()));
            } break;

            default:
                break;
        }

        for (const Ref<UiElement>& child : element->children) {
            WG_CHECKED(exec(child));
        }

        return WG_OK;
    }

    Ref<UiElement> UiQuery::first() const {
        return at_index(0);
    }

    Ref<UiElement> UiQuery::at_index(int i) const {
        if (i >= m_results.size()) {
            return nullptr;
        }
        return m_results[i];
    }

    std::vector<Ref<UiElement>> UiQuery::all() const {
        std::vector<Ref<UiElement>> elements;
        elements.resize(m_results.size());
        std::copy(m_results.begin(), m_results.end(), elements.begin());
        return std::move(elements);
    }

}// namespace wmoge