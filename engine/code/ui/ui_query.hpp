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
#include "core/string_id.hpp"
#include "rtti/class.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiQuery 
     * @brief Ui query allows to search elements within ui tree using selected options
     */
    class UiQuery {
    public:
        UiQuery() = default;

        UiQuery& tag(Strid tag);
        UiQuery& cls(RttiClass* cls);
        UiQuery& multiple();

        [[nodiscard]] Status exec(const Ref<UiElement>& root);

        [[nodiscard]] Ref<UiElement>              first() const;
        [[nodiscard]] Ref<UiElement>              at_index(int i) const;
        [[nodiscard]] std::vector<Ref<UiElement>> all() const;

    private:
        [[nodiscard]] Status exec(UiElement* element);

    private:
        std::optional<Strid>            m_tag;
        std::optional<RttiClass*>       m_cls;
        bool                            m_multiple = false;
        buffered_vector<Ref<UiElement>> m_results;
    };

}// namespace wmoge