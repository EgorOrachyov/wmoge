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
#include "core/array_view.hpp"
#include "core/data.hpp"
#include "io/serialization.hpp"
#include "math/vec.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class Image
     * @brief Hardware-independent image representation.
     *
     * Image class allows to load/save image data, get access to the pixels, manipulate,
     * transform, change data, pass pixel data to rendering-hardware for the rendering.
     */
    class Image : public Asset {
    public:
        WG_RTTI_CLASS(Image, Asset);

        Image()           = default;
        ~Image() override = default;

        /**
         * @brief Creates new image of specified size
         *
         * Allocates data buffer for pixels.
         *
         * @param width image width
         * @param height image height
         * @param channels number of color channels
         * @param pixel_size size in bytes of a pixel
         */
        void create(int width, int height, int channels = 4, int pixel_size = 4);

        /**
         * @brief Loads image from a file
         *
         * @param fs File system to read file
         * @param path Path to image pixel data
         * @param channels Desired number of channels to load (1 to 4)
         *
         * @return True if image successfully loaded
         */
        Status load(class FileSystem* fs, const std::string& path, int channels = 0);

        /**
         * @brief Loads image from a data
         *
         * @param pixel_data Image pixel data loaded from file
         * @param channels Desired number of channels to load (1 to 4)
         *
         * @return True if image successfully loaded
         */
        Status load(array_view<const std::uint8_t> pixel_data, int channels = 0);

        /**
         * @brief Save image to file in file system using specified filepath
         *
         * @param filepath Path with file name to save image
         *
         * @return True if image successfully saved
         */
        Status save(std::filesystem::path filepath);

        /**
         * @brief Resizes image to new dimensions preserving content
         *
         * @param new_width New image width
         * @param new_height New image height
         *
         * @return True if successfully resized
         */
        Status resize(int new_width, int new_height);

        /**
         * @brief Generates mips for this image starting form mip 0 to max mip
         *
         * @param mips List to store generated mips
         *
         * @return True if generated all mips
         */
        Status generate_mip_chain(std::vector<Ref<Image>>& mips);

        [[nodiscard]] const Ref<Data>& get_pixel_data() { return m_pixel_data; }
        [[nodiscard]] int              get_width() { return m_width; }
        [[nodiscard]] int              get_height() { return m_height; }
        [[nodiscard]] int              get_channels() { return m_channels; }
        [[nodiscard]] int              get_pixel_size() { return m_pixel_size; }
        [[nodiscard]] bool             is_empty() { return !(m_width * m_height); }
        [[nodiscard]] bool             is_not_empty() { return m_width * m_height; }
        [[nodiscard]] Vec2i            get_sizei() const { return Vec2i{m_width, m_height}; }
        [[nodiscard]] Vec2f            get_sizef() const { return Vec2f{static_cast<float>(m_width), static_cast<float>(m_height)}; }

        /**
         * @brief Returns max levels (mips including 0) count for specified image
         *
         * @param width Image width; may be 0
         * @param height Image height; may be 0
         * @param depth Image depth; may be 0
         *
         * @return Max image levels count
         */
        static int max_mips_count(int width, int height, int depth);

        /**
         * @brief Returns image size for specified level starting from 0 level
         *
         * @param mip Level to get size of image
         * @param width Image size on level 0
         * @param height Image size on level 0
         *
         * @return Size of the image on requested mip level
         */
        static Size2i mip_size(int mip, int width, int height);

        /**
         * @brief Generates mips for a list of images starting form mip 0 to max mip
         *
         * @param mips List to store generated mips
         *
         * @return True if generated all mips
         */
        static Status generate_mips(const std::vector<Ref<Image>>& images, std::vector<Ref<Image>>& mips);

    private:
        Ref<Data> m_pixel_data;
        int       m_width      = 0;
        int       m_height     = 0;
        int       m_channels   = 0;
        int       m_pixel_size = 0;
    };

    WG_RTTI_CLASS_BEGIN(Image) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_pixel_data, {});
        WG_RTTI_FIELD(m_width, {});
        WG_RTTI_FIELD(m_height, {});
        WG_RTTI_FIELD(m_channels, {});
        WG_RTTI_FIELD(m_pixel_size, {});
    }
    WG_RTTI_END;

    void rtti_grc_image();

}// namespace wmoge