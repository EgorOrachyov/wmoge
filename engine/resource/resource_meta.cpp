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

#include "resource_meta.hpp"

namespace wmoge {

    bool yaml_read(const YamlConstNodeRef& node, ResourceResFile& file) {
        WG_YAML_READ_AS(node, "version", file.version);
        WG_YAML_READ_AS(node, "uuid", file.uuid);
        WG_YAML_READ_AS(node, "class", file.cls);
        WG_YAML_READ_AS(node, "loader", file.loader);
        WG_YAML_READ_AS(node, "deps", file.deps);
        WG_YAML_READ_AS(node, "description", file.description);
        WG_YAML_READ_AS_OPT(node, "path_on_disk", file.path_on_disk);

        return true;
    }

    bool yaml_write(YamlNodeRef node, const ResourceResFile& file) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "version", file.version);
        WG_YAML_WRITE_AS(node, "uuid", file.uuid);
        WG_YAML_WRITE_AS(node, "class", file.cls);
        WG_YAML_WRITE_AS(node, "loader", file.loader);
        WG_YAML_WRITE_AS(node, "deps", file.deps);
        WG_YAML_WRITE_AS(node, "description", file.description);
        WG_YAML_WRITE_AS_OPT(node, "path_on_disk", file.path_on_disk.has_value(), file.path_on_disk);

        return true;
    }

}// namespace wmoge