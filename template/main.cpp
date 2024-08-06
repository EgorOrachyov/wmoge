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

#include <engine.hpp>

#include "assimp_plugin.hpp"
#include "freetype_plugin.hpp"
#include "runtime_plugin.hpp"

namespace wmoge {

    struct TestStruct {
        WG_RTTI_STRUCT(TestStruct);

        std::vector<int> prices = {10, 20, 30};
        std::string      shop   = "uroz";
    };

    WG_RTTI_STRUCT_BEGIN(TestStruct) {
        WG_RTTI_META_DATA(RttiUiName("Test Struct"), RttiUiHint("Rtti test for struct types"));
        WG_RTTI_FIELD(prices, {});
        WG_RTTI_FIELD(shop, {});
    }
    WG_RTTI_END;

    class TestClass : public RttiObject {
        WG_RTTI_CLASS(TestClass, RttiObject);

        void do_work(const TestStruct& a, int b, std::string* c) {
            WG_LOG_INFO("do work " << *c << " " << b);
        }

        std::unordered_map<std::string, TestStruct> map      = {{"1", TestStruct()}};
        std::optional<TestStruct>                   optional = TestStruct();
        Ref<TestClass>                              reference;
    };

    WG_RTTI_CLASS_BEGIN(TestClass) {
        WG_RTTI_META_DATA(RttiUiName("Test Class"), RttiUiHint("Rtti test for class types"));
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(map, {});
        WG_RTTI_FIELD(optional, {});
        WG_RTTI_FIELD(reference, {});
        WG_RTTI_METHOD(do_work, {"a", "b", "c"}, {RttiNoScriptExport});
    }
    WG_RTTI_END;

}// namespace wmoge

using namespace wmoge;

class TemplateApplication : public GameApplication {
public:
    ~TemplateApplication() override = default;

    Status on_register() override {
        PluginManager* plugin_manager = IocContainer::iresolve_v<PluginManager>();
        plugin_manager->add<RuntimePlugin>();
        plugin_manager->add<AssimpPlugin>();
        plugin_manager->add<FreetypePlugin>();

        GameApplication::on_register();

        WG_LOG_INFO("register");
        return StatusCode::Ok;
    }

    Status on_init() override {
        WG_AUTO_PROFILE(app, "TemplateApplication::on_init");

        GameApplication::on_init();

        mesh = Engine::instance()->asset_manager()->load(SID("asset://mesh/suzanne")).cast<Mesh>();

        // Engine::instance()->scene_manager()->change(scene);

        ShaderStructRegister rdc(SID("CanvasDrawCmd"), 80);
        rdc
                .add_field(SID("Transform0"), ShaderTypes::VEC4)
                .add_field(SID("Transform1"), ShaderTypes::VEC4)
                .add_field(SID("Transform2"), ShaderTypes::VEC4)
                .add_field(SID("ClipRect"), ShaderTypes::VEC4)
                .add_field(SID("TextureIdx"), ShaderTypes::INT)
                .add_field(SID("Padding0"), ShaderTypes::INT)
                .add_field(SID("Padding1"), ShaderTypes::INT)
                .add_field(SID("Padding2"), ShaderTypes::INT)
                .finish();

        ShaderStructRegister rdcs(SID("CanvasDrawCmdsBuffer"), 0);
        rdcs
                .add_field_array(SID("DrawCmds"), SID("CanvasDrawCmd"))
                .finish();

        shader = Engine::instance()->asset_manager()->load(SID("engine://shaders/canvas")).cast<Shader>();

        ShaderParamId p_clip_proj_view = shader->find_param_id(SID("ClipProjView"));
        ShaderParamId p_inverse_gamma  = shader->find_param_id(SID("InverseGamma"));

        ShaderParamBlock block(*shader, 0, SID("canvas"));
        block.set_var(p_clip_proj_view, Math3d::perspective(1.0f, 1.0f, 0.1f, 100000.f));
        block.set_var(p_inverse_gamma, 1.0f / 4.0f);
        // block.validate(Engine::instance()->gfx_driver(), Engine::instance()->gfx_ctx());

        Engine* engine = Engine::instance();

        WG_LOG_INFO("init");
        return StatusCode::Ok;
    }

    void debug_draw() {
        WG_AUTO_PROFILE(app, "TemplateApplication::debug_draw");

        Engine*         engine           = Engine::instance();
        AuxDrawManager* aux_draw_manager = engine->aux_draw_manager();

        ShaderPermutation permutation;
        permutation.technique_idx = 0;
        permutation.pass_idx      = 0;

        permutation.options.reset();
        permutation.options.set(0);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f};
        shader->get_or_create_program(GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos2f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f};
        shader->get_or_create_program(GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Uv12f, GfxVertAttrib::Col04f};
        shader->get_or_create_program(GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f, GfxVertAttrib::Col14f};
        shader->get_or_create_program(GfxShaderPlatform::VulkanWindows, permutation);

        permutation.options.reset();
        permutation.options.set(1);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f};
        shader->get_or_create_program(GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos2f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f};
        shader->get_or_create_program(GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Uv12f, GfxVertAttrib::Col04f};
        shader->get_or_create_program(GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f, GfxVertAttrib::Col14f};
        shader->get_or_create_program(GfxShaderPlatform::VulkanWindows, permutation);

        static float angle       = 0.0f;
        static int   frame_count = 0;

        angle += 0.001f;
        frame_count += 1;

        if (Random::next_float() < engine->time()->get_delta_time()) {
            Vec3f   pos      = {Random::next_float(-15, 15), Random::next_float(-5, 5), Random::next_float(10, 20)};
            Color4f color    = {Random::next_float(0.2f, 1), Random::next_float(0.2f, 1), Random::next_float(0.2f, 1), 1};
            Quatf   rot      = {Random::next_float(-5, 5), Random::next_float(-5, 5), Random::next_float(-5, 5)};
            bool    solid    = Random::next_float() < 0.5f;
            float   lifetime = Random::next_float(5, 10);

            aux_draw_manager->draw_box(pos, {1, 1, 1}, color, rot, solid, lifetime);
            aux_draw_manager->draw_line(pos, pos + Vec3f(0, 1.8f, 0), Color::WHITE4f, lifetime);
            aux_draw_manager->draw_text_3d("box", pos + Vec3f(0, 2, 0), 10.0f, Color::RED4f, lifetime);
        }

        aux_draw_manager->draw_text_2d("wmoge engine v0.0", {10, 10}, 15.0f, Color::YELLOW4f);
    }

    Status on_shutdown() override {
        WG_AUTO_PROFILE(app, "TemplateApplication::on_shutdown");

        shader.reset();
        mesh.reset();
        scene.reset();

        GameApplication::on_shutdown();
        WG_LOG_INFO("shutdown");

        return StatusCode::Ok;
    }

    Ref<Scene>    scene;
    WeakRef<Mesh> mesh;
    Ref<Shader>   shader;
};

int main(int argc, const char* const* argv) {
    TemplateApplication app;
    return app.run(argc, argv);
}