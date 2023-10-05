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
        Engine::instance()->scene_manager()->make_active(scene);

        WG_LOG_INFO("init");
    }

    void on_debug_draw() {
        WG_AUTO_PROFILE(app, "GameApplication::on_debug_draw");

        Engine*         engine           = Engine::instance();
        GfxCtx*         gfx_ctx          = engine->gfx_ctx();
        AuxDrawManager* aux_draw_manager = engine->aux_draw_manager();
        WindowManager*  window_manager   = engine->window_manager();
        EcsWorld*       world            = scene->get_ecs_world();

        static float angle       = 0.0f;
        static int   frame_count = 0;

        angle += 0.001f;
        frame_count += 1;

        std::vector<Ref<Camera>> cameras;
        std::vector<Mat4x4f>     transforms;

        EcsQuery query;
        query.set_read<EcsComponentCamera>();
        query.set_read<EcsComponentLocalToWorld>();

        world->each(query, [&](EcsEntity entity) {
            cameras.push_back(world->get_component<EcsComponentCamera>(entity).camera);
            transforms.push_back(world->get_component<EcsComponentLocalToWorld>(entity).matrix);
        });

        int  cam_idx = (frame_count / (60 * 5)) % int(cameras.size());
        auto cam     = cameras[cam_idx];
        auto cam_pos = Math3d::transform(transforms[cam_idx], cam->get_position());
        auto cam_dir = Math3d::transform_w0(transforms[cam_idx], cam->get_direction());
        auto cam_up  = Math3d::transform_w0(transforms[cam_idx], cam->get_up());

        auto viewport = Rect2i{0, 0, window_manager->primary_window()->fbo_width(), window_manager->primary_window()->fbo_height()};
        auto proj     = Math3d::perspective(cam->get_fov(), 1280.0f / 720.0f, 0.1f, 10000.0f);
        auto view     = Math3d::look_at(cam_pos, cam_dir, cam_up);
        auto model    = Math3d::rotate_y(angle) * Math3d::scale(Vec3f(0.25, 0.25, 0.25));

        if (Random::next_float() < engine->get_delta_time()) {
            Vec3f   pos      = {Random::next_float(-5, 5), Random::next_float(-5, 5), Random::next_float(-5, 5)};
            Color4f color    = {Random::next_float(0.2f, 1), Random::next_float(0.2f, 1), Random::next_float(0.2f, 1), 1};
            Quatf   rot      = {Random::next_float(-5, 5), Random::next_float(-5, 5), Random::next_float(-5, 5)};
            bool    solid    = Random::next_float() < 0.5f;
            float   lifetime = Random::next_float(5, 10);

            aux_draw_manager->draw_box(pos, {1, 1, 1}, color, rot, solid, lifetime);
            aux_draw_manager->draw_line(pos, pos + Vec3f(0, 1.8f, 0), Color::WHITE4f, lifetime);
            aux_draw_manager->draw_text_3d("box", pos + Vec3f(0, 2, 0), 10.0f, Color::RED4f, lifetime);
        }

        aux_draw_manager->draw_text_2d("wmoge engine v0.0", {10, 10}, 15.0f, Color::YELLOW4f);

        aux_draw_manager->render(engine->window_manager()->primary_window(), viewport, proj * view, {1280, 720});
        aux_draw_manager->flush(engine->get_delta_time());

        HgfxPassBase pass_solid;
        pass_solid.name          = SID("mesh_solid");
        pass_solid.base_color    = Color::WHITE4f;
        pass_solid.poly_mode     = GfxPolyMode::Fill;
        pass_solid.out_srgb      = true;
        pass_solid.mat_proj_view = proj * view * model;
        pass_solid.attribs_req   = {GfxVertAttrib::Pos3f};
        pass_solid.attribs_full  = {GfxVertAttrib::Pos3f, GfxVertAttrib::Norm3f};
        pass_solid.compile(gfx_ctx);

        HgfxPassBase pass_border;
        pass_border.name          = SID("mesh_border");
        pass_border.base_color    = Color::RED4f;
        pass_border.poly_mode     = GfxPolyMode::Line;
        pass_border.out_srgb      = true;
        pass_border.mat_proj_view = proj * view * model;
        pass_border.attribs_req   = {GfxVertAttrib::Pos3f};
        pass_border.attribs_full  = {GfxVertAttrib::Pos3f, GfxVertAttrib::Norm3f};
        pass_border.compile(gfx_ctx);

        gfx_ctx->begin_render_pass({}, SID("quad"));
        {
            gfx_ctx->bind_target(engine->window_manager()->primary_window());
            gfx_ctx->viewport(viewport);

            auto draw_mesh = [&]() {
                gfx_ctx->bind_vert_buffer(mesh->get_gfx_vertex_buffer(0), 0, 0);

                for (auto& chunk : mesh->get_chunks()) {
                    gfx_ctx->bind_index_buffer(mesh->get_gfx_index_buffer(), wmoge::GfxIndexType::Uint32, chunk.index_offset);
                    gfx_ctx->draw_indexed(chunk.index_count, 0, 1);
                }
            };

            if (pass_solid.configure(gfx_ctx)) {
                draw_mesh();
            }
            if (pass_border.configure(gfx_ctx)) {
                draw_mesh();
            }
        }
        gfx_ctx->end_render_pass();
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