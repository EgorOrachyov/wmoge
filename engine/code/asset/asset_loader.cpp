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

#include "asset_loader.hpp"

namespace wmoge {

    void AssetLoadRequest::add_data_file(const Strid& name) {
        data_files[name] = name;
    }

    void AssetLoadRequest::add_data_file(const Strid& name, const std::string& path) {
        data_files[name] = SID(path);
    }

    Strid AssetLoadRequest::get_data_file(Strid tag) const {
        auto query = data_files.find(tag);
        return query != data_files.end() ? query->second : Strid();
    }

    void AssetLoadResult::add_data_file(Strid tag, array_view<const std::uint8_t> data) {
        data_files[tag] = data;
    }

    array_view<const std::uint8_t> AssetLoadResult::get_data_file(Strid tag) const {
        auto query = data_files.find(tag);
        return query != data_files.end() ? query->second : array_view<const std::uint8_t>();
    }

}// namespace wmoge