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
#include "core/flat_map.hpp"
#include "core/mask.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "core/var.hpp"

#include <initializer_list>
#include <vector>

namespace wmoge {

    /**
     * @brief List of built-in meta attributes
    */
    enum class RttiAttributeType {
        NoSaveLoad,    // Property must not be saved and loaded in serialization
        NoCopy,        // Property must not be copied on duplication
        NoScriptExport,// Property must not be exported to scrip binding
        Optional,      // Property optional to load from textual data
        Inline,        // Property must not be saved and loaded iline
        UiName,        // Ui friendly name
        UiHint,        // Ui hint for the user
        UiCategory,    // Ui category for the search
        UiInline,      // Ui inline content of field into parent in view
        UiHidden,      // Ui view is hidden from user
        Custom         // Custom attribute
    };

    /**
     * @class RttiAttribute
     * @brief Base class for any rtti attribute
    */
    class RttiAttribute : public RefCnt {
    public:
        RttiAttribute(RttiAttributeType type) : m_type(type) {}
        virtual ~RttiAttribute() = default;

        [[nodiscard]] RttiAttributeType get_type() const { return m_type; }

    private:
        const RttiAttributeType m_type;
    };

    /**
     * @class RttiMetaData
     * @brief Meta data associated with a type or its internal member
    */
    class RttiMetaData {
    public:
        RttiMetaData() = default;
        RttiMetaData(const std::initializer_list<Ref<RttiAttribute>>& attributes);

        [[nodiscard]] bool is_no_save_load() const;
        [[nodiscard]] bool is_no_copy() const;
        [[nodiscard]] bool is_no_script_exprot() const;
        [[nodiscard]] bool is_optional() const;
        [[nodiscard]] bool is_inline() const;
        [[nodiscard]] bool has_attribute_of_type(RttiAttributeType type);

        [[nodiscard]] array_view<const Ref<RttiAttribute>> get_attributes() const { return m_attributes; }

    private:
        std::vector<Ref<RttiAttribute>> m_attributes;
        Mask<RttiAttributeType>         m_flags;
    };

    /** @brief Rtti attribute template wrapper */
    template<RttiAttributeType type>
    class RttiAttributeBaseT : public RttiAttribute {
    public:
        RttiAttributeBaseT() : RttiAttribute(type) {}
    };

    /** @brief Rtti attribute template wrapper */
    template<RttiAttributeType type>
    class RttiAttributeBaseStringT : public RttiAttribute {
    public:
        RttiAttributeBaseStringT(std::string value) : RttiAttribute(type), m_value(std::move(value)) {}

        [[nodiscard]] const std::string& get_value() const { return m_value; }

    private:
        std::string m_value;
    };

    using RttiAttributeNoSaveLoad     = RttiAttributeBaseT<RttiAttributeType::NoSaveLoad>;
    using RttiAttributeNoCopy         = RttiAttributeBaseT<RttiAttributeType::NoCopy>;
    using RttiAttributeNoScriptExport = RttiAttributeBaseT<RttiAttributeType::NoScriptExport>;
    using RttiAttributeOptional       = RttiAttributeBaseT<RttiAttributeType::Optional>;
    using RttiAttributeInline         = RttiAttributeBaseT<RttiAttributeType::Inline>;
    using RttiAttributeUiName         = RttiAttributeBaseStringT<RttiAttributeType::UiName>;
    using RttiAttributeUiHint         = RttiAttributeBaseStringT<RttiAttributeType::UiHint>;
    using RttiAttributeUiCategory     = RttiAttributeBaseStringT<RttiAttributeType::UiCategory>;
    using RttiAttributeUiInline       = RttiAttributeBaseT<RttiAttributeType::UiInline>;
    using RttiAttributeUiHidden       = RttiAttributeBaseT<RttiAttributeType::UiHidden>;

#define RttiNoSaveLoad      make_ref<RttiAttributeNoSaveLoad>().as<RttiAttribute>()
#define RttiNoCopy          make_ref<RttiAttributeNoCopy>().as<RttiAttribute>()
#define RttiNoScriptExport  make_ref<RttiAttributeNoScriptExport>().as<RttiAttribute>()
#define RttiOptional        make_ref<RttiAttributeOptional>().as<RttiAttribute>()
#define RttiInline          make_ref<RttiAttributeInline>().as<RttiAttribute>()
#define RttiUiName(str)     make_ref<RttiAttributeUiName>(str).as<RttiAttribute>()
#define RttiUiHint(str)     make_ref<RttiAttributeUiHint>(str).as<RttiAttribute>()
#define RttiUiCategory(str) make_ref<RttiAttributeUiCategory>(str).as<RttiAttribute>()
#define RttiUiInline        make_ref<RttiAttributeUiInline>().as<RttiAttribute>()
#define RttiUiHidden        make_ref<RttiAttributeUiHidden>().as<RttiAttribute>()

}// namespace wmoge