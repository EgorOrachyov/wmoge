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

#ifndef WMOGE_SHADER_2D_HPP
#define WMOGE_SHADER_2D_HPP

#include "math/mat.hpp"
#include "math/vec.hpp"
#include "resource/material_shader.hpp"

namespace wmoge {

    /**
     * @class Shader2d
     * @brief Custom shader to draw 2d shapes in overlay pass
     */
    class Shader2d : public MaterialShader {
    public:
        WG_OBJECT(Shader2d, MaterialShader)

        struct PassParams {
            Mat4x4f clip_proj_view;
            Mat4x4f clip_proj_view_prev;
            Vec2f   view_size;
            float   time;
            float   time_dt;
        };
        static_assert(!(sizeof(PassParams) % sizeof(float[4])), "invalid alignment");

        struct DrawParams {
            Mat4x4f model;
            Mat4x4f model_prev;
            Mat4x4f model_inv;
            Mat4x4f model_inv_prev;
            Vec4f   tint;
            int     layer_id;
            int     pad[3];
        };
        static_assert(!(sizeof(DrawParams) % sizeof(float[4])), "invalid alignment");

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;

    protected:
        void on_build_add_defines(ShaderBuilder& builder) override;
        void on_build_add_params(ShaderBuilder& builder) override;
        void on_build_add_main(ShaderBuilder& builder) override;
    };

}// namespace wmoge

#endif//WMOGE_SHADER_2D_HPP
