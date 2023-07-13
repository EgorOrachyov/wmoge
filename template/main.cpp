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

class EcsSystemMultMatr : public EcsSystem {
public:
    ~EcsSystemMultMatr() override = default;

    void process_batch(EcsWorld& world, EcsArchStorage& storage, int start_entity, int count) override { WG_ECS_SYSTEM_BIND(EcsSystemMultMatr); }

    [[nodiscard]] EcsSystemType     get_type() const override { return EcsSystemType::Default; }
    [[nodiscard]] EcsSystemExecMode get_exec_mode() const override { return EcsSystemExecMode::OnWorkers; }
    [[nodiscard]] StringId          get_name() const override { return SID("EcsSystemMultMart"); }
    [[nodiscard]] EcsQuery          get_query() const override {
        EcsQuery query;
        query.set_read<EcsLtW>();
        query.set_write<EcsWtL>();
        return query;
    }

private:
    void process(EcsWorld& world, const EcsEntity& entity, const EcsLtW& ltw, EcsWtL& wtl) {
        WG_AUTO_PROFILE_SCENE("EcsSystemMultMatr::process");

        Mat4x4f t = Math3d::rotate(Vec3f::axis_y(), Random::next_float());

        wtl.matrix = ltw.matrix * t;
    }
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

        Engine::instance()->file_system()->watch("root://shaders");
        Engine::instance()->event_manager()->subscribe(make_listener<EventFileSystem>([](const EventFileSystem& event) {
            WG_LOG_INFO("fs " << Enum::to_str(event.action) << " "
                              << " " << event.path
                              << " " << event.entry);
            return false;
        }));

        WG_LOG_INFO("init");
    }

    void on_update() override {
        WG_AUTO_PROFILE(app, "GameApplication::on_update");

        Engine*    engine     = Engine::instance();
        GfxDriver* gfx_driver = engine->gfx_driver();
        GfxCtx*    gfx_ctx    = engine->gfx_ctx();

        static float angle = 0.0f;

        angle += 0.01f;

        auto proj  = Math3d::perspective(Math::deg_to_rad(90.0f), 1280.0f / 720.0f, 0.1f, 10000.0f);
        auto view  = Math3d::look_at({0, 0, 3}, {0, 0, -1}, {0, 1, 0});
        auto model = Math3d::rotate_y(angle);

        struct Element {
            Vec3f pos;
            Vec4f col0;
            Vec4f col1;
        };

        auto verts   = gfx_driver->dyn_vert_buffer()->allocate_n<Element>(3);
        verts.ptr[0] = {{-1, 1, 0}, {1, 1, 1, 1}, {1, 0, 0, 1}};
        verts.ptr[1] = {{-1, -1, 0}, {1, 1, 1, 1}, {1, 0, 0, 1}};
        verts.ptr[2] = {{1, -1, 0}, {1, 1, 1, 1}, {1, 0, 0, 1}};
        verts.ptr[3] = {{1, 1, 0}, {1, 1, 1, 1}, {1, 0, 0, 1}};
        gfx_driver->dyn_vert_buffer()->flush();

        auto indices   = gfx_driver->dyn_index_buffer()->allocate_n<std::uint16_t>(6);
        indices.ptr[0] = 0;
        indices.ptr[1] = 1;
        indices.ptr[2] = 2;
        indices.ptr[3] = 2;
        indices.ptr[4] = 3;
        indices.ptr[5] = 0;
        gfx_driver->dyn_index_buffer()->flush();

        HgfxPassBase pass_solid;
        pass_solid.name           = SID("quad-solid");
        pass_solid.mix_weights[0] = 0.0f;
        pass_solid.poly_mode      = GfxPolyMode::Fill;
        pass_solid.out_srgb       = true;
        pass_solid.gamma          = 2.2f;
        pass_solid.mat_proj_view  = proj * view * model;
        pass_solid.attribs        = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f, GfxVertAttrib::Col14f};
        pass_solid.depth_enable   = false;
        pass_solid.compile(gfx_ctx);

        HgfxPassBase pass_border;
        pass_border.name           = SID("quad-border");
        pass_border.mix_weights[0] = 1.0f;
        pass_border.poly_mode      = GfxPolyMode::Line;
        pass_border.out_srgb       = true;
        pass_border.gamma          = 2.2f;
        pass_border.mat_proj_view  = proj * view * model;
        pass_border.attribs        = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f, GfxVertAttrib::Col14f};
        pass_border.depth_enable   = false;
        pass_border.compile(gfx_ctx);

        gfx_ctx->begin_render_pass({}, SID("quad"));
        {
            gfx_ctx->bind_target(engine->window_manager()->primary_window());
            gfx_ctx->clear(0, Color::BLACK4f);
            gfx_ctx->clear(1.0f, 0);
            gfx_ctx->viewport(Rect2i{0, 0, 1280, 720});

            if (pass_solid.configure(gfx_ctx)) {
                gfx_ctx->bind_vert_buffer(Ref<GfxVertBuffer>(verts.buffer), 0, verts.offset);
                gfx_ctx->bind_index_buffer(Ref<GfxIndexBuffer>(indices.buffer), GfxIndexType::Uint16, indices.offset);
                gfx_ctx->draw_indexed(6, 0, 1);
            }

            if (pass_border.configure(gfx_ctx)) {
                gfx_ctx->bind_vert_buffer(Ref<GfxVertBuffer>(verts.buffer), 0, verts.offset);
                gfx_ctx->bind_index_buffer(Ref<GfxIndexBuffer>(indices.buffer), GfxIndexType::Uint16, indices.offset);
                gfx_ctx->draw_indexed(6, 0, 1);
            }
        }
        gfx_ctx->end_render_pass();
    }

    void on_shutdown() override {
        {
            EcsRegistry* registry = Engine::instance()->ecs_registry();
            registry->register_component<EcsLtW>();
            registry->register_component<EcsWtL>();

            EcsArch arch;
            arch.set_component<EcsLtW>();
            arch.set_component<EcsWtL>();

            std::shared_ptr<EcsSystem> system = std::make_shared<EcsSystemMultMatr>();

            EcsWorld world;
            world.register_system(system);

            const int N = 200;

            for (int i = 0; i < N; i++) {
                EcsEntity entity = world.allocate_entity();
                world.make_entity(entity, arch);
            }

            world.execute_system(system);
        }

        Application::on_shutdown();
        WG_LOG_INFO("shutdown");
    }
};

int main(int argc, const char* const* argv) {
    GameApplication game_application;
    return game_application.run(argc, argv);
}