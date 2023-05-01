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

#ifndef WMOGE_SHADER_HPP
#define WMOGE_SHADER_HPP

#include "core/fast_map.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "render/shader_builder.hpp"
#include "render/shader_variant.hpp"
#include "resource/mesh.hpp"
#include "resource/resource.hpp"

#include <mutex>

namespace wmoge {

    /**
     * @class Shader
     * @brief Shader program for a rendering
     *
     * Shader is special resource which consists of a vertex and fragment
     * code written using glsl language. This code if followed by a special
     * declaration, which defines the shader domain, render queue type and
     * set of data and texture parameters, which are exposed by this shader
     * to the end (material) user.
     *
     * @note For actual rendering shader can produce one or more shader variants.
     *       Single variant is an actual gfx shader item, key and hash. Variants
     *       share common behaviour, but differ in a set of defines. Variants
     *       created on demand, when they are requested.
     *
     * @note Variants creation is optimized by usage of the shader cache. If
     *       an item was compiled once and its byte code for current platform
     *       was cached, then its byte code reused.
     *
     * @see ShaderVariant
     */
    class Shader : public Resource {
    public:
        WG_OBJECT(Shader, Resource);

        /**
         * @brief Make shader from glsl sources of vertex and fragment code
         * Used primary for internal usage and loading of engine shaders.
         *
         * @param vertex Vertex shader source code
         * @param fragment Fragment shader source code
         *
         * @return True on success
         */
        bool create_from_source(const std::string& vertex, const std::string& fragment);

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;

        bool           has_variant(const StringId& key);
        ShaderVariant* find_variant(const StringId& key);
        ShaderVariant* create_variant(const fast_vector<std::string>& defines);
        ShaderVariant* create_variant(MeshAttribs mesh_attribs, fast_vector<std::string> defines);

        const std::string& get_vertex();
        const std::string& get_fragment();

    protected:
        virtual void on_build(ShaderBuilder& builder);

    protected:
        fast_map<StringId, ShaderVariant> m_variants;// Compiled and cached variants of the shader
        std::string                       m_vertex;
        std::string                       m_fragment;
        std::mutex                        m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_SHADER_HPP
