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

#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "core/uuid.hpp"
#include "rtti/traits.hpp"

#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class PluginConfig
     * @brief Class holding config information of a plugin
    */
    struct PluginConfig {
        WG_RTTI_STRUCT(PluginConfig)

        Strid                    id;
        Strid                    class_name;
        UUID                     uuid;
        int                      version = 0;
        std::string              version_name;
        std::string              name;
        std::string              created_by;
        std::string              category;
        std::string              brief;
        std::string              description;
        std::vector<Strid>       depends;
        std::vector<Strid>       excludes;
        std::vector<Strid>       search_tags;
        std::vector<std::string> change_notes;
    };

    WG_RTTI_STRUCT_BEGIN(PluginConfig) {
        WG_RTTI_META_DATA({});
        WG_RTTI_FIELD(id, {});
        WG_RTTI_FIELD(uuid, {RttiOptional});
        WG_RTTI_FIELD(version, {RttiOptional});
        WG_RTTI_FIELD(version_name, {RttiOptional});
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(created_by, {RttiOptional});
        WG_RTTI_FIELD(category, {RttiOptional});
        WG_RTTI_FIELD(brief, {RttiOptional});
        WG_RTTI_FIELD(depends, {RttiOptional});
        WG_RTTI_FIELD(excludes, {RttiOptional});
        WG_RTTI_FIELD(search_tags, {RttiOptional});
        WG_RTTI_FIELD(change_notes, {RttiOptional});
    }
    WG_RTTI_END;

}// namespace wmoge