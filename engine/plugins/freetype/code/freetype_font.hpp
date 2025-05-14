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

#include "core/array_view.hpp"
#include "core/status.hpp"
#include "core/string_utils.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/font.hpp"
#include "grc/texture_manager.hpp"
#include "platform/file_system.hpp"

namespace wmoge {

    /**
     * @class FreetypeFont
     * @brief Provides utilies to work with .ttf font via freetype library
    */
    class FreetypeFont {
    public:
        FreetypeFont(class IocContainer* ioc);

        /**
         * @brief Loads font from a .ttf file from file system using specified height in pixels
         *
         * @note Uses FreeType2 library for .ttf file loading
         *
         * @param font Font to load into
         * @param ttf_data Content of the font .ttf file in a file system
         * @param height Font height in pixels
         * @param glyphs_in_row Num of glyphs in a row of a bitmap
         *
         * @return True if font loaded
         */
        Status load(const Ref<Font>& font, array_view<const std::uint8_t> ttf_data, int height = 40, int glyphs_in_row = 16);

    private:
        GfxDriver*      m_gfx_driver;
        FileSystem*     m_file_system;
        TextureManager* m_texture_manager;
    };

    /**
     * @class FreetypeFontLoader
     * @brief Provides utilies to work with .ttf font via freetype library
    */
    class FreetypeFontLoader {
    public:
        /**
         * @brief Loads font from a .ttf file from file system using specified height in pixels
         *
         * @note Uses FreeType2 library for .ttf file loading
         *
         * @param path Path of font file
         * @param font_desc Font desc to be loaded
         * @param bitmap_desc Bitmap texture desc
         * @param ttf_data Content of the font .ttf file in a file system
         * @param height Font height in pixels
         * @param glyphs_in_row Num of glyphs in a row of a bitmap
         *
         * @return True if font loaded
         */
        Status load(const std::string& path, FontDesc& font_desc, TextureDesc& bitmap_desc, array_view<const std::uint8_t> ttf_data, int height = 40, int glyphs_in_row = 16);
    };

}// namespace wmoge