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

#ifndef WMOGE_RESOURCE_LOADER_DEFAULT_HPP
#define WMOGE_RESOURCE_LOADER_DEFAULT_HPP

#include "resource/resource.hpp"
#include "resource/resource_loader.hpp"
#include "resource/resource_meta.hpp"
#include "resource/resource_pak.hpp"

namespace wmoge {

    /**
     * @class ResourceLoaderDefault
     * @brief Default-loader for resources from import options
     */
    class ResourceLoaderDefault final : public ResourceLoader {
    public:
        ~ResourceLoaderDefault() override = default;
        bool     load(const StringId& name, const ResourceMeta& meta, Ref<Resource>& res) override;
        bool     can_load(const StringId& resource_type) override;
        StringId get_name() override;
    };

}// namespace wmoge

#endif//WMOGE_RESOURCE_LOADER_DEFAULT_HPP
