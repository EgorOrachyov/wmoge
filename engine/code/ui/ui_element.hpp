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
#include "ui/ui_defs.hpp"

#include <string>

namespace wmoge {

    /**
     * @class UiElement
     * @brief Base class for all ui elements
     */
    class UiElement : public RefCnt {
    public:
        virtual ~UiElement() = default;

        void set_name(std::string name) { m_name = std::move(name); }
        void set_enabled(bool enabled) { m_enabled = enabled; }

        [[nodiscard]] const std::string& get_name() const { return m_name; }
        [[nodiscard]] bool               get_enabled() const { return m_enabled; }

    protected:
        std::string m_name;
        bool        m_enabled = true;
    };

    /**
     * @class UiSubElement
     * @brief Base class for all ui sub-elements which could be nested
     */
    class UiSubElement : public UiElement {
    public:
        ~UiSubElement() override = default;

        void set_hint_width(UiHintWidth hint) { m_hint_width = hint; }

        [[nodiscard]] const UiHintWidth& get_hint_width() const { return m_hint_width; }

    protected:
        UiHintWidth m_hint_width;
    };

}// namespace wmoge