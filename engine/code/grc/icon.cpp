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

#include "icon.hpp"

namespace wmoge {

    void IconAtlas::set_desc(IconAtlasDesc desc) {
        m_desc = std::move(desc);
    }

    const IconInfo& IconAtlas::get_icon_info(int id) const {
        return m_desc.icons[id];
    }

    const IconAtlasPage& IconAtlas::get_page(int id) const {
        return m_desc.pages[id];
    }

    std::optional<class Icon> IconAtlas::try_find_icon(Strid name) {
        auto query = m_desc.icons_map.find(name);
        if (query != m_desc.icons_map.end()) {
            return Icon(Ref<IconAtlas>(this), query->second);
        }
        return std::nullopt;
    }

    Icon::Icon(AssetRef<IconAtlas> atlas, int id)
        : m_atlas(std::move(atlas)),
          m_id(id) {
    }

    void rtti_grc_icon() {
        rtti_type<IconInfo>();
        rtti_type<IconAtlasPage>();
        rtti_type<IconAtlasDesc>();
        rtti_type<IconAtlas>();
        rtti_type<Icon>();
    }

}// namespace wmoge