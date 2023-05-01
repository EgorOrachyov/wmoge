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

    Archive& operator<<(Archive& archive, const GfxShaderReflection::Texture& texture) {
        archive << texture.name;
        archive << texture.set;
        archive << texture.binding;
        archive << texture.array_size;
        archive << static_cast<int>(texture.tex);
        return archive;
    }
    Archive& operator>>(Archive& archive, GfxShaderReflection::Texture& texture) {
        archive >> texture.name;
        archive >> texture.set;
        archive >> texture.binding;
        archive >> texture.array_size;
        int tex;
        archive >> tex;
        texture.tex = static_cast<GfxTex>(tex);
        return archive;
    }
    Archive& operator<<(Archive& archive, const GfxShaderReflection::Buffer& buffer) {
        archive << buffer.name;
        archive << buffer.set;
        archive << buffer.binding;
        archive << buffer.size;
        return archive;
    }
    Archive& operator>>(Archive& archive, GfxShaderReflection::Buffer& buffer) {
        archive >> buffer.name;
        archive >> buffer.set;
        archive >> buffer.binding;
        archive >> buffer.size;
        return archive;
    }
    Archive& operator<<(Archive& archive, const GfxShaderReflection& reflection) {
        WG_AUTO_PROFILE_GFX();

        archive << reflection.textures;
        archive << reflection.ub_buffers;
        archive << reflection.sb_buffers;
        archive << reflection.textures_per_desc;
        archive << reflection.ub_buffers_per_desc;
        archive << reflection.sb_buffers_per_desc;
        return archive;
    }
    Archive& operator>>(Archive& archive, GfxShaderReflection& reflection) {
        WG_AUTO_PROFILE_GFX();

        archive >> reflection.textures;
        archive >> reflection.ub_buffers;
        archive >> reflection.sb_buffers;
        archive >> reflection.textures_per_desc;
        archive >> reflection.ub_buffers_per_desc;
        archive >> reflection.sb_buffers_per_desc;
        return archive;
    }

}// namespace wmoge