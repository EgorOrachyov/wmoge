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

#ifndef WMOGE_SHADER_CACHE_HPP
#define WMOGE_SHADER_CACHE_HPP

#include "core/fast_map.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_shader.hpp"
#include "io/archive.hpp"

#include <mutex>
#include <string>

namespace wmoge {

    /**
     * @class ShaderCache
     * @brief Runtime and offline cache of compiled gfx shaders
     *
     * Shader cache allows to reuse created shaders at runtime. Also it allows
     * to speed up shader creation significantly (from 0.5-1.0 sec to 10ms) by
     * caching the shader byte code and saving it on a disk for a reuse on
     * a next game run.
     */
    class ShaderCache {
    public:
        ShaderCache();
        ~ShaderCache();

        Ref<GfxShader> find(const std::string& key);
        void           cache(const std::string& key, Ref<GfxShader> shader);
        void           clear();
        void           save(const std::string& path);
        void           load(const std::string& path);

        /**
         * @class ShaderData
         * @brief Entry holding data of a particular shader
         */
        struct ShaderData {
            StringId       name;
            Ref<GfxShader> shader;
            Ref<Data>      bytecode;

            friend Archive& operator<<(Archive& archive, const ShaderData& shader_data);
            friend Archive& operator>>(Archive& archive, ShaderData& shader_data);
        };

    private:
        fast_map<std::string, ShaderData> m_entries;
        std::mutex                        m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_SHADER_CACHE_HPP
