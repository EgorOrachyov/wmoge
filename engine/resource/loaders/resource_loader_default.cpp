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

#include "resource_loader_default.hpp"

#include "debug/profiler.hpp"

namespace wmoge {

    bool ResourceLoaderDefault::load(const StringId& name, const ResourceMeta& meta, Ref<Resource>& res) {
        WG_AUTO_PROFILE_RESOURCE("ResourceLoaderDefault::load");

        res = meta.cls->instantiate().cast<Resource>();
        res->set_name(name);

        if (!res) {
            WG_LOG_ERROR("failed to instantiate resource " << name);
            return false;
        }

        if (meta.path_on_disk->empty()) {
            WG_LOG_ERROR("no path on disk to load resource file " << name);
            return false;
        }

        auto resource_tree = yaml_parse_file(meta.path_on_disk.value());

        if (resource_tree.empty()) {
            WG_LOG_ERROR("failed to read parse file " << meta.path_on_disk.value());
            return false;
        }

        res->set_name(name);

        if (!res->load_from_yaml(resource_tree.crootref())) {
            WG_LOG_ERROR("failed to load resource from file " << meta.path_on_disk.value());
            return false;
        }

        return true;
    }
    StringId ResourceLoaderDefault::get_name() {
        return SID("default");
    }

}// namespace wmoge