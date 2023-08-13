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

#ifndef WMOGE_RESOURCE_META_HPP
#define WMOGE_RESOURCE_META_HPP

#include "core/class.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "core/uuid.hpp"
#include "io/yaml.hpp"
#include "resource/resource.hpp"

#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @class ResourceMeta
     * @brief Meta information of a particular resource
     */
    struct ResourceMeta {
        int                        version = 0;
        UUID                       uuid    = UUID();
        class Class*               cls     = nullptr;
        class ResourcePak*         pak     = nullptr;
        class ResourceLoader*      loader  = nullptr;
        fast_vector<StringId>      deps;
        std::optional<std::string> path_on_disk;
        std::optional<YamlTree>    import_options;
    };

    /**
     * @class ResourceResFile
     * @brief Structure for ResourceMeta info stored as `.res` file in file system
     */
    struct ResourceResFile {
        int                        version;
        UUID                       uuid;
        StringId                   cls;
        StringId                   loader;
        fast_vector<StringId>      deps;
        std::string                description;
        std::optional<std::string> path_on_disk;

        friend Status yaml_read(const YamlConstNodeRef& node, ResourceResFile& file);
        friend Status yaml_write(YamlNodeRef node, const ResourceResFile& file);
    };

}// namespace wmoge

#endif//WMOGE_RESOURCE_META_HPP
