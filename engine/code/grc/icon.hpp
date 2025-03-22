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

#include "asset/asset.hpp"
#include "asset/asset_ref.hpp"
#include "core/flat_map.hpp"
#include "grc/texture.hpp"
#include "math/color.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class IconInfo
     * @brief Single icon info
     */
    struct IconInfo {
        WG_RTTI_STRUCT(IconInfo)

        Strid   name;
        int     id      = -1;
        int     page_id = -1;
        Vec2f   uv_pos{0, 0};
        Vec2f   uv_size{1, 1};
        Vec2f   pixels{32, 32};
        Color4f tint = Color::WHITE4f;
    };

    WG_RTTI_STRUCT_BEGIN(IconInfo) {
        WG_RTTI_META_DATA({});
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(page_id, {});
        WG_RTTI_FIELD(uv_pos, {});
        WG_RTTI_FIELD(uv_size, {});
        WG_RTTI_FIELD(pixels, {});
        WG_RTTI_FIELD(tint, {});
    }
    WG_RTTI_END;

    /**
     * @class IconAtlasPage
     * @brief Icons atlas page with single gpu texture resource
     */
    struct IconAtlasPage {
        WG_RTTI_STRUCT(IconAtlasPage)

        AssetRef<Texture2d>          texture;
        std::vector<AssetRef<Image>> source_images;
    };

    WG_RTTI_STRUCT_BEGIN(IconAtlasPage) {
        WG_RTTI_META_DATA({});
        WG_RTTI_FIELD(texture, {});
        WG_RTTI_FIELD(source_images, {});
    }
    WG_RTTI_END;

    /**
     * @class IconAtlasDesc
     * @brief Desc structure to init icons atlas
     */
    struct IconAtlasDesc {
        WG_RTTI_STRUCT(IconAtlasDesc)

        flat_map<Strid, int>       icons_map;
        std::vector<IconInfo>      icons;
        std::vector<IconAtlasPage> pages;
    };

    WG_RTTI_STRUCT_BEGIN(IconAtlasDesc) {
        WG_RTTI_META_DATA({});
        WG_RTTI_FIELD(icons_map, {});
        WG_RTTI_FIELD(icons, {});
        WG_RTTI_FIELD(pages, {});
    }
    WG_RTTI_END;

    /**
     * @class IconAtlas
     * @brief Atlas storing collection of icons packed inside atlas pages
     */
    class IconAtlas : public Asset {
    public:
        WG_RTTI_CLASS(IconAtlas, Asset)

        void                      set_desc(IconAtlasDesc desc);
        const IconInfo&           get_icon_info(int id) const;
        const IconAtlasPage&      get_page(int id) const;
        std::optional<class Icon> try_find_icon(Strid name);

    private:
        IconAtlasDesc m_desc;
    };

    WG_RTTI_CLASS_BEGIN(IconAtlas) {
        WG_RTTI_META_DATA({});
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_desc, {});
    }
    WG_RTTI_END;

    /**
     * @class Icon
     * @brief Shortcut to a simple icon stored within icons atlas
     */
    class Icon {
    public:
        WG_RTTI_STRUCT(Icon)

        Icon() = default;
        Icon(AssetRef<IconAtlas> atlas, int id);

        [[nodiscard]] const AssetRef<IconAtlas>& get_atlas() const { return m_atlas; }
        [[nodiscard]] const IconInfo&            get_info() const { return m_atlas->get_icon_info(m_id); }
        [[nodiscard]] int                        get_id() const { return m_id; }
        [[nodiscard]] bool                       is_empty() const { return !m_atlas; }

    private:
        AssetRef<IconAtlas> m_atlas;
        int                 m_id = -1;
    };

    WG_RTTI_STRUCT_BEGIN(Icon) {
        WG_RTTI_META_DATA({});
        WG_RTTI_FIELD(m_atlas, {});
        WG_RTTI_FIELD(m_id, {});
    }
    WG_RTTI_END;

}// namespace wmoge