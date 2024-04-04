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
#include "image_plugin.hpp"

using namespace wmoge;

class TemplateApplication : public GameApplication {
public:
    ~TemplateApplication() override = default;

    Status on_register() override {
        PluginManager* plugin_manager = IocContainer::instance()->resolve_v<PluginManager>();
        plugin_manager->add<AssimpPlugin>();
        plugin_manager->add<FreetypePlugin>();
        plugin_manager->add<ImagePlugin>();

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

        model = Engine::instance()->resource_manager()->load(SID("res://models/suzanne")).cast<Model>();

        auto scene_tree_packed = Engine::instance()->resource_manager()->load(SID("res://trees/test_scene")).cast<SceneTreePacked>();
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

        auto shader  = Engine::instance()->resource_manager()->load(SID("res://shaders/test_shader")).cast<Shader>();
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
        YamlTree            yaml_tree = yaml_parse_file("res://../../shaders/canvas.shader");
        WG_YAML_READ(yaml_tree.crootref(), shader_script_file);

        shader_manager->load_script(shader_script_file);

        Ref<GrcShaderScript> shader_script    = shader_manager->find_script(SID("canvas"));
        GrcShaderParamId     p_clip_proj_view = shader_script->get_param_id(SID("ClipProjView"));
        GrcShaderParamId     p_inverse_gamma  = shader_script->get_param_id(SID("InverseGamma"));

        GrcShaderParamBlock block(*shader_script, 0);
        block.set_var(p_clip_proj_view, Math3d::perspective(1.0f, 1.0f, 0.1f, 100000.f));
        block.set_var(p_inverse_gamma, 1.0f / 4.0f);
        block.validate(Engine::instance()->gfx_driver(), Engine::instance()->gfx_ctx(), SID("test"));

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