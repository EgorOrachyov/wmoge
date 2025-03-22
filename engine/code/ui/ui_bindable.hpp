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

#include "rtti/object.hpp"
#include "rtti/traits.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_markup.hpp"

namespace wmoge {

    /** @brief Info passed to a bindable instance on binding */
    struct UiBindInfo {
        std::function<void(Strid)>       notify_changed;
        std::function<UiElement*(Strid)> find_element;
        UiElement*                       root_element = nullptr;
    };

    /**
     * @class UiBindable
     * @brief Base class for programming code behind ui declared in markup files
     */
    class UiBindable : public RttiObject {
    public:
        WG_RTTI_CLASS(UiBindable, RttiObject);

        virtual Status on_bind() { return WG_OK; }

        void                     set_bind_info(UiBindInfo info);
        void                     notify_changed(Strid poperty_id) const;
        [[nodiscard]] UiElement* find_element_by(Strid tag) const;
        [[nodiscard]] UiElement* get_root_element() const;

    private:
        UiBindInfo m_bind_info;
    };

    WG_RTTI_CLASS_BEGIN(UiBindable) {
        WG_RTTI_FACTORY();
    };
    WG_RTTI_END;

}// namespace wmoge