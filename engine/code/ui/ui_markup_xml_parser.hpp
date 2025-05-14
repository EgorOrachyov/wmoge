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

#include "core/array_view.hpp"
#include "core/string_utils.hpp"
#include "rtti/type_storage.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_markup.hpp"

#include <tinyxml2.hpp>

namespace wmoge {

    /**
     * @class UiMarkupParser
     * @brief Parses xml markup document into a ui markup desc object
     */
    class UiMarkupParser {
    public:
    public:
        UiMarkupParser(std::string name, UiMarkupDecs& desc, array_view<const std::uint8_t> xml_buffer, RttiTypeStorage* type_storage);

        [[nodiscard]] Status parse();

    private:
        [[nodiscard]] Status parse_element(tinyxml2::XMLElement* xml_node, Ref<UiElement>& out);
        [[nodiscard]] Status parse_binding(const tinyxml2::XMLElement* xml_binding, Ref<UiElement>& out);
        [[nodiscard]] Status parse_attribute(const tinyxml2::XMLAttribute* xml_attribute, Ref<UiElement>& out, RttiClass* cls);

    private:
        std::string                    m_name;
        UiMarkupDecs&                  m_desc;
        tinyxml2::XMLDocument          m_document;
        array_view<const std::uint8_t> m_xml_buffer;
        RttiTypeStorage*               m_type_storage;
    };

}// namespace wmoge
