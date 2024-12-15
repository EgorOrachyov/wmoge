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
#include "core/mask.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "core/var.hpp"

#include <initializer_list>

namespace wmoge {

    /**
     * @brief List of available meta attributes
    */
    enum class RttiMetaAttribute {
        NoSaveLoad,    // Property must not be saved and loaded in serialization
        NoCopy,        // Property must not be copied on duplication
        NoScriptExport,// Property must not be exported to scrip binding
        Optional,      // Property optional to load from textual data
        Inline,        // Property must not be saved and loaded iline
        UiName,        // Ui friendly name
        UiHint,        // Ui hint for the user
        UiCategory,    // Ui category for the search
        UiInline,      // Ui inline content of field into parent in view
        UiHidden       // Ui view is hidden from user
    };

    /**
     * @class RttiMetaProperty
     * @brief Holds meta attribute with value
    */
    struct RttiMetaProperty {
        RttiMetaProperty(RttiMetaAttribute attribute) : attribute(attribute), value(Var()) {}
        RttiMetaProperty(RttiMetaAttribute attribute, Var value) : attribute(attribute), value(std::move(value)) {}

        const RttiMetaAttribute attribute;
        const Var               value;
    };

    /**
     * @class RttiMetaData
     * @brief Meta data associated with a type or its internal member
    */
    class RttiMetaData {
    public:
        RttiMetaData() = default;
        RttiMetaData(const std::initializer_list<const RttiMetaProperty>& properties_list);

        [[nodiscard]] bool is_no_save_load() const;
        [[nodiscard]] bool is_no_copy() const;
        [[nodiscard]] bool is_no_script_exprot() const;
        [[nodiscard]] bool is_optional() const;
        [[nodiscard]] bool is_inline() const;
        [[nodiscard]] bool has_attribute(RttiMetaAttribute attribute);

        [[nodiscard]] const flat_map<RttiMetaAttribute, Var>& get_properties() const { return m_properties; }

    private:
        flat_map<RttiMetaAttribute, Var> m_properties;
        Mask<RttiMetaAttribute>          m_attributes;
    };

#define RttiNoSaveLoad      RttiMetaProperty(RttiMetaAttribute::NoSaveLoad)
#define RttiNoCopy          RttiMetaProperty(RttiMetaAttribute::NoCopy)
#define RttiNoScriptExport  RttiMetaProperty(RttiMetaAttribute::NoScriptExport)
#define RttiOptional        RttiMetaProperty(RttiMetaAttribute::Optional)
#define RttiInline          RttiMetaProperty(RttiMetaAttribute::Inline)
#define RttiUiName(str)     RttiMetaProperty(RttiMetaAttribute::UiName, Strid(str))
#define RttiUiHint(str)     RttiMetaProperty(RttiMetaAttribute::UiName, std::string(str))
#define RttiUiCategory(str) RttiMetaProperty(RttiMetaAttribute::UiName, Strid(str))
#define RttiUiInline        RttiMetaProperty(RttiMetaAttribute::UiInline)
#define RttiUiHidden        RttiMetaProperty(RttiMetaAttribute::UiHidden)

}// namespace wmoge