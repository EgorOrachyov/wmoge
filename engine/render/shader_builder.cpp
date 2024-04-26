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

#include "shader_builder.hpp"

#include "gfx/gfx_driver.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

namespace wmoge {

    void ShaderBuilder::configure_vs() {
        vertex = std::make_optional(std::stringstream{});
    }
    void ShaderBuilder::configure_fs() {
        fragment = std::make_optional(std::stringstream{});
    }
    void ShaderBuilder::configure_cs() {
        compute = std::make_optional(std::stringstream{});
    }
    void ShaderBuilder::add_define(const std::string& define) {
        add_define_vs(define);
        add_define_fs(define);
        add_define_cs(define);
    }
    void ShaderBuilder::add_defines(const buffered_vector<std::string>& defines) {
        for (const auto& d : defines) {
            add_define(d);
        }
    }
    void ShaderBuilder::add_define_vs(const std::string& define) {
        if (vertex.has_value()) {
            vertex.value() << "#define " << define << "\n";
        }
    }
    void ShaderBuilder::add_define_fs(const std::string& define) {
        if (fragment.has_value()) {
            fragment.value() << "#define " << define << "\n";
        }
    }
    void ShaderBuilder::add_define_cs(const std::string& define) {
        if (compute.has_value()) {
            compute.value() << "#define " << define << "\n";
        }
    }
    void ShaderBuilder::add_vs_module(const std::string& code) {
        if (vertex.has_value()) {
            vertex.value() << code;
        }
    }
    void ShaderBuilder::add_fs_module(const std::string& code) {
        if (fragment.has_value()) {
            fragment.value() << code;
        }
    }
    void ShaderBuilder::add_cs_module(const std::string& code) {
        if (compute.has_value()) {
            compute.value() << code;
        }
    }

    bool ShaderBuilder::compile() {
        WG_AUTO_PROFILE_RENDER("ShaderBuilder::compile");

        Engine*    engine     = Engine::instance();
        GfxDriver* gfx_driver = engine->gfx_driver();

        if (vertex.has_value() && fragment.has_value()) {
            gfx_shader = gfx_driver->make_shader(vertex->str(), fragment->str(), layouts, key);
            return true;
        }

        if (compute.has_value()) {
            gfx_shader = gfx_driver->make_shader(compute->str(), layouts, key);
            return true;
        }

        WG_LOG_ERROR("unknown shader modules combination");
        return false;
    }

}// namespace wmoge