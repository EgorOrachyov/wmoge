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

#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_shader.hpp"
#include "material/shader.hpp"
#include "platform/file_system.hpp"

namespace wmoge {

    class ShaderDesc : public RefCnt {
    };

    /**
     * @class ShaderPass
     * @brief Base class for any engine shading pass
     *
     * Shader pass is a high level shading program representation. It provides connection
     * between a shader, written using glsl in engine source code, optional user defined
     * material and a low-level engine gfx api.
     *
     * Shader pass provides info about required pipeline layout, allows to obtain final
     * shader source code, provides defines info and etc.
     */
    class ShaderPass {
    public:
        virtual ~ShaderPass() = default;

        /**
         * @brief Compile a gfx shader for this specific shader pass
         *
         * @param name Shader unique name
         * @param driver Gfx driver for assets creation
         * @param attribs Vertex used attributes in the shader
         * @param defines Additional defines for compilation
         * @param shader Optional material user shader
         * @param out_shader Out gfx shader for actual rendering
         *
         * @return Ok on success
         */
        virtual Status compile(const Strid& name, GfxDriver* driver, const GfxVertAttribs& attribs, const buffered_vector<std::string>& defines, class Shader* shader, Ref<GfxShader>& out_shader);

        /**
         * @brief Reloads shader pass sources from a disc
         *
         * @param folder Folder directory where shaders are
         * @param file_system File system adapter to use for loading
         *
         * @return Ok on success
         */
        virtual Status reload_sources(const std::string& folder, FileSystem* file_system) = 0;

        /** @brief fills pipeline required layout with optional shader passed for material assets */
        virtual void fill_layout(GfxDescSetLayoutDescs& layouts_desc, Shader* shader){};

        /** @brief get vertex shader code if present */
        virtual const std::string& get_vertex(GfxShaderLang lang) = 0;

        /** @brief get fragment shader code if present */
        virtual const std::string& get_fragment(GfxShaderLang lang) = 0;

        /** @brief get compute shader code if present */
        virtual const std::string& get_compute(GfxShaderLang lang) = 0;

        /** @brief unique lower-case pass name */
        virtual Strid get_name() = 0;
    };

}// namespace wmoge