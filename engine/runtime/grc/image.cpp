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

#include "image.hpp"

#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <stb_image.hpp>
#include <stb_image_resize.hpp>
#include <stb_image_write.hpp>

namespace wmoge {

    void Image::create(int width, int height, int channels, int pixel_size) {
        WG_AUTO_PROFILE_ASSET("Image::create");

        if (!width || !height || !pixel_size || !channels) {
            WG_LOG_ERROR("an attempt to make empty image");
            return;
        }

        m_width      = width;
        m_height     = height;
        m_channels   = channels;
        m_pixel_size = pixel_size;
        m_pixel_data = make_ref<Data>(width * height * pixel_size);
    }
    Status Image::load(FileSystem* fs, const std::string& path, int channels) {
        WG_AUTO_PROFILE_ASSET("Image::load");

        std::vector<std::uint8_t> pixel_data;
        WG_CHECKED(fs->read_file(path, pixel_data));

        return load(pixel_data, channels);
    }
    Status Image::load(array_view<const std::uint8_t> pixel_data, int channels) {
        WG_AUTO_PROFILE_ASSET("Image::load");

        int w, h, n;
        channels = Math::clamp(channels, 0, 4);

        stbi_uc* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(pixel_data.data()), static_cast<int>(pixel_data.size()), &w, &h, &n, channels);
        if (!data) {
            WG_LOG_ERROR("failed to read image data");
            return StatusCode::FailedRead;
        }

        m_width      = w;
        m_height     = h;
        m_channels   = channels ? channels : n;
        m_pixel_size = int(m_channels * sizeof(stbi_uc));
        m_pixel_data = make_ref<Data>(data, w * h * m_pixel_size);

        stbi_image_free(data);

        return WG_OK;
    }
    Status Image::save(std::filesystem::path filepath) {
        WG_AUTO_PROFILE_ASSET("Image::save");

        if (!m_width || !m_height) {
            WG_LOG_ERROR("cannot save empty image");
            return StatusCode::InvalidData;
        }

        auto filepath_str = filepath.replace_extension("png").string();
        return stbi_write_png(filepath_str.c_str(), m_width, m_height, m_channels, m_pixel_data->buffer(), m_width * m_pixel_size) ? StatusCode::Ok : StatusCode::Error;
    }
    Status Image::resize(int new_width, int new_height) {
        WG_AUTO_PROFILE_ASSET("Image::resize");

        if (!new_width || !new_height) {
            WG_LOG_ERROR("cannot resize image " << get_name() << " to " << new_width << "x" << new_height);
            return StatusCode::InvalidData;
        }

        auto new_size       = new_width * new_height * m_pixel_size;
        auto new_pixel_data = make_ref<Data>(new_size);

        auto ok = stbir_resize_uint8(reinterpret_cast<const unsigned char*>(m_pixel_data->buffer()), m_width, m_height, m_width * m_pixel_size,
                                     reinterpret_cast<unsigned char*>(new_pixel_data->buffer()), new_width, new_height, new_width * m_pixel_size, m_channels);

        if (!ok) {
            WG_LOG_ERROR("failed to resize image " << get_name() << " to " << new_width << "x" << new_height);
            return StatusCode::Error;
        }

        m_width      = new_width;
        m_height     = new_height;
        m_pixel_data = new_pixel_data;

        return WG_OK;
    }

    Status Image::generate_mip_chain(std::vector<Ref<Image>>& mips) {
        WG_AUTO_PROFILE_ASSET("Image::generate_mip_chain");

        assert(mips.empty());

        mips.push_back(duplicate().cast<Image>());
        int mips_count = max_mips_count(m_width, m_height, 1);
        for (int i = 1; i < mips_count; i++) {
            mips.push_back(mips[i - 1]->duplicate().cast<Image>());
            Size2i mip_size = Image::mip_size(i, m_width, m_height);
            if (!mips.back()->resize(mip_size[0], mip_size[1]))
                return StatusCode::Error;
        }

        return WG_OK;
    }

    int Image::max_mips_count(int width, int height, int depth) {
        int max_dim = Math::max(width, Math::max(height, depth));
        return static_cast<int>(Math::floor(Math::log2(static_cast<float>(max_dim)))) + 1;
    }

    Size2i Image::mip_size(int mip, int width, int height) {
        while (mip > 0) {
            if (width > 1) width = width / 2;
            if (height > 1) height = height / 2;
            mip -= 1;
        }
        return Size2i(width, height);
    }

}// namespace wmoge
