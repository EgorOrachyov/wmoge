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

struct EcsLtW : EcsComponent {
    WG_ECS_COMPONENT(EcsLtW, 0);

    Mat4x4f matrix = Math3d::identity();
};

struct EcsWtL : EcsComponent {
    WG_ECS_COMPONENT(EcsWtL, 1);

    Mat4x4f matrix = Math3d::identity();
};

class GameApplication : public Application {
public:
    ~GameApplication() override = default;

    void on_register() override {
        Application::on_register();
        WG_LOG_INFO("register");
    }

    void on_init() override {
        Application::on_init();

        Engine::instance()->action_manager()->load_action_map("root://actions/actionmap_console.yml");
        Engine::instance()->action_manager()->enable_action_map(SID("console"));

        EcsRegistry* registry = Engine::instance()->ecs_registry();
        registry->register_component<EcsLtW>();
        registry->register_component<EcsWtL>();

        EcsWorld  world;
        EcsEntity entity = world.allocate_entity();
        EcsArch   arch;
        arch.set_component<EcsLtW>();
        arch.set_component<EcsWtL>();

        world.make_entity(entity, arch);

        auto& ltw = world.get_component_rw<EcsLtW>(entity);
        auto& wtl = world.get_component_rw<EcsWtL>(entity);

        WG_LOG_INFO(entity);
        WG_LOG_INFO(arch);

        world.destroy_entity(entity);

        WG_LOG_INFO("init");
    }

    void on_shutdown() override {
        Application::on_shutdown();
        WG_LOG_INFO("shutdown");
    }
};

int main(int argc, const char* const* argv) {
    GameApplication game_application;
    return game_application.run(argc, argv);
}