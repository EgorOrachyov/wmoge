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

#ifndef WMOGE_RESOURCE_PAK_HPP
#define WMOGE_RESOURCE_PAK_HPP

#include "core/data.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "io/yaml.hpp"
#include "resource/resource.hpp"
#include "resource/resource_meta.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class ResourcePak
     * @brief Interface for the package of the resources on disc
     *
     * ResourcePak abstracts access to the resources on disk. It provides ability
     * to load a particular resource meta file from a resource name, and allows
     * to read a raw data using path.
     *
     * Internally resource pack can be represented as a wrapper for a file system
     * resource directory, or it can manage a compressed pak of resources on a disk.
     */
    class ResourcePak {
    public:
        virtual ~ResourcePak()                                                                  = default;
        virtual std::string get_name() const                                                    = 0;
        virtual Status      get_meta(const ResourceId& name, ResourceMeta& meta)                = 0;
        virtual Status      read_file(const std::string& path, Ref<Data>& data)                 = 0;
        virtual Status      read_file(const std::string& path, std::vector<std::uint8_t>& data) = 0;
    };

}// namespace wmoge

#endif//WMOGE_RESOURCE_PAK_HPP
