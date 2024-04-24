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
        WG_RTTI_STRUCT(TestStruct)

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
        WG_RTTI_CLASS(TestClass, RttiObject)

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
        PluginManager* plugin_manager = IocContainer::instance()->resolve_v<PluginManager>();
        plugin_manager->add<RuntimePlugin>();
        plugin_manager->add<AssimpPlugin>();
        plugin_manager->add<FreetypePlugin>();

        GameApplication::on_register();

        WG_LOG_INFO("register");
        return StatusCode::Ok;
    }

    Status on_init() override {
        GameApplication::on_init();

        Engine::instance()->action_manager()->load("root://actions/actionmap_console.yml");
        Engine::instance()->action_manager()->load("root://actions/actionmap_camera_debug.yml");
        Engine::instance()->action_manager()->activate(SID("console"));
        Engine::instance()->action_manager()->activate(SID("camera_debug"));

        model = Engine::instance()->asset_manager()->load(SID("asset://models/suzanne")).cast<Model>();

        auto scene_tree_packed = Engine::instance()->asset_manager()->load(SID("asset://trees/test_scene")).cast<SceneTreePacked>();
        scene_tree             = scene_tree_packed->instantiate();
        scene                  = scene_tree->get_scene();

        Engine::instance()->scene_manager()->change(scene);

        class ApplicationLayer : public Layer {
        public:
            ApplicationLayer(TemplateApplication* game) : game(game) {}

            void on_iter() override {
                game->debug_draw();
            }

            TemplateApplication* game;
        };

        Engine::instance()->layer_stack()->attach(std::make_shared<ApplicationLayer>(this));

        auto shader  = Engine::instance()->asset_manager()->load(SID("asset://shaders/test_shader")).cast<Shader>();
        auto variant = shader->create_variant({GfxVertAttribs{GfxVertAttrib::Pos3f}}, {"MESH_PASS_GBUFFER"});

        while (variant->status() == GfxShaderStatus::Compiling) {
            WG_LOG_INFO("waiting for " << variant->name() << " compilation...");
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }
        WG_LOG_INFO("status for " << variant->name() << " is " << magic_enum::enum_name(variant->status()));

        GrcShaderStructRegister rdc(SID("DrawCmd"), 4 * 4 * 3 + 4 * 4 + 4 * 4);
        rdc
                .add_field(SID("Transform0"), GrcShaderTypes::VEC4)
                .add_field(SID("Transform1"), GrcShaderTypes::VEC4)
                .add_field(SID("Transform2"), GrcShaderTypes::VEC4)
                .add_field(SID("ClipRect"), GrcShaderTypes::VEC4)
                .add_field(SID("TextureIdx0"), GrcShaderTypes::INT)
                .add_field(SID("TextureIdx1"), GrcShaderTypes::INT)
                .add_field(SID("TextureIdx2"), GrcShaderTypes::INT)
                .add_field(SID("TextureIdx3"), GrcShaderTypes::INT)
                .finish();

        GrcShaderStructRegister rdcs(SID("DrawCmdsBuffer"), 0);
        rdcs
                .add_field_array(SID("DrawCmds"), SID("DrawCmd"))
                .finish();

        GrcShaderManager* shader_manager = IocContainer::instance()->resolve_v<GrcShaderManager>();

        GrcShaderScriptFile shader_script_file;
        YamlTree            yaml_tree = yaml_parse_file("asset://../../shaders/canvas.shader");
        WG_YAML_READ(yaml_tree.crootref(), shader_script_file);

        shader_manager->load_script(shader_script_file);

        Ref<GrcShaderScript> shader_script    = shader_manager->find_script(SID("canvas"));
        GrcShaderParamId     p_clip_proj_view = shader_script->get_param_id(SID("ClipProjView"));
        GrcShaderParamId     p_inverse_gamma  = shader_script->get_param_id(SID("InverseGamma"));

        GrcShaderParamBlock block(*shader_script, 0);
        block.set_var(p_clip_proj_view, Math3d::perspective(1.0f, 1.0f, 0.1f, 100000.f));
        block.set_var(p_inverse_gamma, 1.0f / 4.0f);
        block.validate(Engine::instance()->gfx_driver(), Engine::instance()->gfx_ctx(), SID("test"));

        Engine* engine = Engine::instance();

        std::vector<GfxFormat> formats = {GfxFormat::BC1_RGB, GfxFormat::BC2, GfxFormat::R16};
        std::vector<Vec2i>     sizes   = {Vec2i(10, 20), Vec2i(0, 30), Vec2i(-1, -1)};

        RttiType*        tenum   = rtti_type<GfxSampAddress>();
        RttiType*        tint    = rtti_type<int>();
        RttiType*        tfloat  = rtti_type<float>();
        RttiType*        tstring = rtti_type<std::string>();
        RttiType*        tstrid  = rtti_type<Strid>();
        RttiType*        tvecf   = rtti_type<decltype(formats)>();
        RttiType*        tvecs   = rtti_type<decltype(sizes)>();
        RttiType*        tstruct = rtti_type<TestStruct>();
        RttiType*        tclass  = rtti_type<TestClass>();
        RttiTypeStorage* s       = RttiTypeStorage::instance();

        std::stringstream stream;

        stream.clear();
        tvecf->to_string(&formats, stream);
        WG_LOG_INFO(stream.str());

        stream.str("");
        tvecs->add_element(&sizes);
        tvecs->remove_element(&sizes, 0);
        tvecs->remove_element(&sizes, 0);
        tvecs->to_string(&sizes, stream);
        WG_LOG_INFO(stream.str());

        stream.str("");
        TestStruct test_struct;
        tstruct->to_string(&test_struct, stream);
        WG_LOG_INFO(stream.str());

        stream.str("");
        TestClass test_class;
        tclass->to_string(&test_class, stream);
        WG_LOG_INFO(stream.str());

        TestStruct  a;
        int         b = 2;
        std::string c = "rdr";

        std::uint8_t arg_buffer[20];
        *((TestStruct**) (arg_buffer + 0))   = &a;
        *((int*) (arg_buffer + 8))           = b;
        *((std::string**) (arg_buffer + 12)) = &c;

        RttiFrame frame;

        RttiFunction* do_work = test_class.get_class()->find_method(SID("do_work")).value()->get_function().get();
        do_work->call(frame, &test_class, arg_buffer);
        WG_LOG_INFO("call " << do_work->get_name());

        WG_LOG_INFO("init");
        return StatusCode::Ok;
    }

    void debug_draw() {
        WG_AUTO_PROFILE(app, "GameApplication::debug_draw");

        Engine*         engine           = Engine::instance();
        AuxDrawManager* aux_draw_manager = engine->aux_draw_manager();

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
        model.reset();
        mesh.reset();
        scene.reset();
        scene_tree.reset();

        GameApplication::on_shutdown();
        WG_LOG_INFO("shutdown");

        return StatusCode::Ok;
    }

    Ref<Scene>     scene;
    Ref<SceneTree> scene_tree;
    WeakRef<Mesh>  mesh;
    Ref<Model>     model;
};

int main(int argc, const char* const* argv) {
    TemplateApplication app;
    return app.run(argc, argv);
}