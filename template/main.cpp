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

using namespace wmoge;

class GameApplication : public Application {
public:
    ~GameApplication() override = default;

    void on_register() override {
        Application::on_register();
        WG_LOG_INFO("register");
    }

    void on_init() override {
        Application::on_init();

        Engine::instance()->action_manager()->load("root://actions/actionmap_console.yml");
        Engine::instance()->action_manager()->load("root://actions/actionmap_camera_debug.yml");
        Engine::instance()->action_manager()->activate(SID("console"));
        Engine::instance()->action_manager()->activate(SID("camera_debug"));

        mesh = Engine::instance()->resource_manager()->load(SID("res://mesh/suzanne")).cast<Mesh>();

        Ref<SceneTreePacked> scene_tree_packed = Engine::instance()->resource_manager()->load(SID("res://trees/test_scene")).cast<SceneTreePacked>();
        scene_tree                             = scene_tree_packed->instantiate();
        scene                                  = scene_tree->get_scene();

        class ApplicationLayer : public Layer {
        public:
            ApplicationLayer(GameApplication* game) : game(game) {}

            void on_debug_draw() override {
                game->on_debug_draw();
            }

            GameApplication* game;
        };

        Engine::instance()->layer_stack()->attach(std::make_shared<ApplicationLayer>(this));
        Engine::instance()->scene_manager()->change(scene);

        auto shader  = Engine::instance()->resource_manager()->load(SID("res://shaders/test_shader")).cast<Shader>();
        auto variant = shader->create_variant({GfxVertAttribs{GfxVertAttrib::Pos3f}}, {"MESH_PASS_GBUFFER"});

        while (variant->status() == GfxShaderStatus::Compiling) {
            WG_LOG_INFO("waiting for " << variant->name() << " compilation...");
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }
        WG_LOG_INFO("status for " << variant->name() << " is " << magic_enum::enum_name(variant->status()));

        WG_LOG_INFO("init");
    }

    void on_debug_draw() {
        WG_AUTO_PROFILE(app, "GameApplication::on_debug_draw");

        Engine*         engine           = Engine::instance();
        AuxDrawManager* aux_draw_manager = engine->aux_draw_manager();

        static float angle       = 0.0f;
        static int   frame_count = 0;

        angle += 0.001f;
        frame_count += 1;

        if (Random::next_float() < engine->get_delta_time()) {
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

    void on_shutdown() override {
        mesh.reset();
        scene.reset();
        scene_tree.reset();

        Application::on_shutdown();
        WG_LOG_INFO("shutdown");
    }

    Ref<Scene>     scene;
    Ref<SceneTree> scene_tree;
    Ref<Mesh>      mesh;
};

int main(int argc, const char* const* argv) {
    GameApplication game_application;
    return game_application.run(argc, argv);
}