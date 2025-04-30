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

#include "asset/asset.hpp"
#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "core/var.hpp"
#include "ui/ui_defs.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /** @brief Ui layout desc, which describes template ui eleement */
    struct UiMarkupDecs {
        WG_RTTI_STRUCT(UiMarkupDecs);

        Ref<UiElement> root;
    };

    WG_RTTI_STRUCT_BEGIN(UiMarkupDecs) {
        WG_RTTI_FIELD(root, {});
    }
    WG_RTTI_END;

    /**
     * @class UiMarkup
     * @brief Ui layout and structure which can be loaded from disc and instantiated
     */
    class UiMarkup : public Asset {
    public:
        WG_RTTI_CLASS(UiMarkup, Asset);

        void           set_desc(UiMarkupDecs desc) { m_desc = std::move(desc); }
        Ref<UiElement> make_elements() const { return m_desc.root->duplicate().cast<UiElement>(); }

        [[nodiscard]] const UiMarkupDecs& get_desc() const { return m_desc; }

    private:
        UiMarkupDecs m_desc;
    };

    WG_RTTI_CLASS_BEGIN(UiMarkup) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_desc, {});
    }
    WG_RTTI_END;

}// namespace wmoge