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

#include "gfx_shader.hpp"

#include "debug/profiler.hpp"

namespace wmoge {

    Status archive_write(Archive& archive, const GfxShaderReflection::Texture& texture) {
        WG_AUTO_PROFILE_GFX("GfxShaderReflection::Texture::Archive<<");

        WG_ARCHIVE_WRITE(archive, texture.name);
        WG_ARCHIVE_WRITE(archive, texture.set);
        WG_ARCHIVE_WRITE(archive, texture.binding);
        WG_ARCHIVE_WRITE(archive, texture.array_size);
        WG_ARCHIVE_WRITE(archive, texture.tex);

        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, GfxShaderReflection::Texture& texture) {
        WG_AUTO_PROFILE_GFX("GfxShaderReflection::Texture::Archive>>");

        WG_ARCHIVE_READ(archive, texture.name);
        WG_ARCHIVE_READ(archive, texture.set);
        WG_ARCHIVE_READ(archive, texture.binding);
        WG_ARCHIVE_READ(archive, texture.array_size);
        WG_ARCHIVE_READ(archive, texture.tex);

        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const GfxShaderReflection::Buffer& buffer) {
        WG_AUTO_PROFILE_GFX("GfxShaderReflection::Buffer::Archive<<");

        WG_ARCHIVE_WRITE(archive, buffer.name);
        WG_ARCHIVE_WRITE(archive, buffer.set);
        WG_ARCHIVE_WRITE(archive, buffer.binding);
        WG_ARCHIVE_WRITE(archive, buffer.size);

        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, GfxShaderReflection::Buffer& buffer) {
        WG_AUTO_PROFILE_GFX("GfxShaderReflection::Buffer::Archive>>");

        WG_ARCHIVE_READ(archive, buffer.name);
        WG_ARCHIVE_READ(archive, buffer.set);
        WG_ARCHIVE_READ(archive, buffer.binding);
        WG_ARCHIVE_READ(archive, buffer.size);

        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const GfxShaderReflection& reflection) {
        WG_AUTO_PROFILE_GFX("GfxShaderReflection::Archive<<");

        WG_ARCHIVE_WRITE(archive, reflection.textures);
        WG_ARCHIVE_WRITE(archive, reflection.ub_buffers);
        WG_ARCHIVE_WRITE(archive, reflection.sb_buffers);
        WG_ARCHIVE_WRITE(archive, reflection.textures_per_desc);
        WG_ARCHIVE_WRITE(archive, reflection.ub_buffers_per_desc);
        WG_ARCHIVE_WRITE(archive, reflection.sb_buffers_per_desc);

        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, GfxShaderReflection& reflection) {
        WG_AUTO_PROFILE_GFX("GfxShaderReflection::Archive>>");

        WG_ARCHIVE_READ(archive, reflection.textures);
        WG_ARCHIVE_READ(archive, reflection.ub_buffers);
        WG_ARCHIVE_READ(archive, reflection.sb_buffers);
        WG_ARCHIVE_READ(archive, reflection.textures_per_desc);
        WG_ARCHIVE_READ(archive, reflection.ub_buffers_per_desc);
        WG_ARCHIVE_READ(archive, reflection.sb_buffers_per_desc);

        return StatusCode::Ok;
    }

}// namespace wmoge