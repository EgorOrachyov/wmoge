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

#include "resource_pak_fs.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "platform/file_system.hpp"

namespace wmoge {

    ResourcePakFileSystem::ResourcePakFileSystem() {
        m_file_system = Engine::instance()->file_system();
    }
    bool ResourcePakFileSystem::meta(const StringId& name, ResourceMeta& meta) {
        WG_AUTO_PROFILE_RESOURCE();

        std::string               meta_file_path = name.str() + ".res";
        std::vector<std::uint8_t> meta_file;

        if (!m_file_system->read_file(meta_file_path, meta_file)) return false;

        auto ryml_tree = Yaml::parse(meta_file);
        auto deps      = ryml_tree["deps"];

        meta.deps.reserve(deps.num_children());

        for (auto entry = deps.first_child(); entry.valid(); entry = entry.next_sibling()) {
            std::string dependency;
            entry >> dependency;
            meta.deps.push_back(std::move(SID(dependency)));
        }

        std::string class_name;
        std::string loader_name;
        ryml_tree["class"] >> class_name;
        ryml_tree["loader"] >> loader_name;

        meta.resource_class = Class::class_ptr(SID(class_name));
        meta.pak            = this;
        meta.loader         = SID(loader_name);
        meta.path_on_disk   = m_file_system->resolve(meta_file_path);
        meta.import_options.emplace(std::move(ryml_tree));

        return true;
    }
    bool ResourcePakFileSystem::read_file(const std::string& path, ref_ptr<Data>& data) {
        return m_file_system->read_file(path, data);
    }
    bool ResourcePakFileSystem::read_file(const std::string& path, std::vector<std::uint8_t>& data) {
        return m_file_system->read_file(path, data);
    }

}// namespace wmoge
