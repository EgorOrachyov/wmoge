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

// Flappy owl game with wmoge game engine
// Game logic is scripted using lua
class FlappyOwl final : public Application {
public:
    void reload() {
        debug_camera = false;

        // Globals
        auto engine           = Engine::instance();
        auto scene_manager    = engine->scene_manager();
        auto resource_manager = engine->resource_manager();

        // Force resource cache freeing to reload fresh from disk
        resource_manager->clear();

        // Load scene packed resource
        auto scene_packed = resource_manager->load(SID("res://scenes/main")).cast<ScenePacked>();

        // Instantiate scene (also possible to do it async)
        auto scene = scene_packed->instantiate();

        // Schedule scene to run
        scene_manager->next_running(scene);

        // Shutdown prev scene (must be done explicitly due to cyclic deps on scene)
        if (scene_manager->get_running_scene()) {
            scene_manager->shutdown_scene(scene_manager->get_running_scene());
        }

        WG_LOG_INFO("load scene");
    }

    void on_init() override {
        Application::on_init();

        reload();

        auto* console = Engine::instance()->console();

        // Allow game reload from a console
        console->register_cmd(SID("reload_shaders"), "reload_shaders game scene", [this](auto&) {
            reload();
            return 0;
        });

        // Toggle on/off debug camera to see more info
        console->register_cmd(SID("toggle_debug"), "toggle debug camera on/off", [this](auto&) {
            auto  scene        = Engine::instance()->scene_manager()->get_running_scene();
            auto* camera       = scene->get_child(1)->get<Camera2d>();
            auto  screen_space = camera->get_screen_space();
            screen_space       = debug_camera ? screen_space * 0.5f : screen_space * 2.0f;
            debug_camera       = !debug_camera;
            camera->set_screen_space(screen_space);
            return 0;
        });

        WG_LOG_INFO("init game");
    }

    void on_shutdown() override {
        Application::on_shutdown();
        WG_LOG_INFO("shutdown game");
    }

    bool debug_camera = false;
};

int main(int argc, const char* const* argv) {
    FlappyOwl flappy_owl;
    return flappy_owl.run(argc, argv);
}