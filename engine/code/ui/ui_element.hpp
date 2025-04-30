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
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "rtti/traits.hpp"
#include "rtti/type_ref.hpp"
#include "ui/ui_defs.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class UiBinding
     * @brief Stores info on how to bind data source property to ui element property
     */
    struct UiBinding {
        WG_RTTI_STRUCT(UiBinding);

        Strid         property;
        Strid         data_path;
        UiBindingType type = UiBindingType::ToTarget;
    };

    WG_RTTI_STRUCT_BEGIN(UiBinding) {
        WG_RTTI_FIELD(property, {});
        WG_RTTI_FIELD(data_path, {});
        WG_RTTI_FIELD(type, {});
    }
    WG_RTTI_END;

    /** @brief An instance of binding with bound data source */
    using UiBindingUpdater = std::function<void()>;

    /**
     * @class UiElement
     * @brief Base class for all ui elements
     */
    class UiElement : public RttiObject {
    public:
        WG_RTTI_CLASS(UiElement, RttiObject);

        UiElement(UiElementType type) : type(type) {}
        UiElement() = default;

        Strid                         tag;
        Strid                         sub_style;
        std::optional<UiCursorType>   cursor;
        Ref<RefCnt>                   user_data;
        RttiRefClass                  data_source_type;
        Ref<RttiObject>               data_source;
        std::vector<Ref<UiElement>>   children;
        std::vector<UiBinding>        bindings;
        std::vector<UiBindingUpdater> bindings_updater;
        UiElementType                 type = UiElementType::Unknown;
    };

    WG_RTTI_CLASS_BEGIN(UiElement) {
        WG_RTTI_FIELD(tag, {});
        WG_RTTI_FIELD(sub_style, {});
        WG_RTTI_FIELD(cursor, {});
        WG_RTTI_FIELD(data_source_type, {});
        WG_RTTI_FIELD(data_source, {RttiNoSaveLoad});
        WG_RTTI_FIELD(children, {});
        WG_RTTI_FIELD(bindings, {});
        WG_RTTI_FIELD(type, {});
    }
    WG_RTTI_END;

}// namespace wmoge