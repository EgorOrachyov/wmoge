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

#include "meta_data.hpp"

namespace wmoge {

    RttiMetaData::RttiMetaData(const std::initializer_list<Ref<RttiAttribute>>& attributes) {
        m_attributes.reserve(attributes.size());
        for (const auto& attribute : attributes) {
            m_attributes.push_back(attribute);
            m_flags.set(attribute->get_type());
        }
    }

    bool RttiMetaData::is_no_save_load() const {
        return m_flags.get(RttiAttributeType::NoSaveLoad);
    }
    bool RttiMetaData::is_no_copy() const {
        return m_flags.get(RttiAttributeType::NoCopy);
    }
    bool RttiMetaData::is_no_script_exprot() const {
        return m_flags.get(RttiAttributeType::NoScriptExport);
    }
    bool RttiMetaData::is_optional() const {
        return m_flags.get(RttiAttributeType::Optional);
    }
    bool RttiMetaData::is_inline() const {
        return m_flags.get(RttiAttributeType::Inline);
    }
    bool RttiMetaData::has_attribute_of_type(RttiAttributeType type) {
        return m_flags.get(type);
    }

}// namespace wmoge