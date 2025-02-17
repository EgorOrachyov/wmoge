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

#pragma once

#include <engine.hpp>

#include "io/stream_file.hpp"
#include "io/tree_yaml.hpp"
#include "render/shader_funcs.hpp"

using namespace wmoge;

class TemplateGame : public GamePlugin {
public:
    TemplateGame() {
    }

    Status on_init() override {
        WG_PROFILE_CPU_SCOPE(app, "TemplateGame::on_init");

        WG_CHECKED(GamePlugin::on_init());

        auto async_font  = m_engine->asset_manager()->load_async(SID("assets/fonts/consolas"));
        auto async_mesh  = m_engine->asset_manager()->load_async(SID("assets/mesh/suzanne"));
        auto async_tex2d = m_engine->asset_manager()->load_async(SID("assets/textures/dirt_mask"));
        // auto async_texCube = m_engine->asset_manager()->load_async(SID("assets/textures/skybox"));

        async_font.wait_completed();
        async_mesh.wait_completed();
        async_tex2d.wait_completed();
        // async_texCube.wait_completed();

        font  = async_font.result().cast<Font>();
        mesh  = async_mesh.result().cast<Mesh>();
        tex2d = async_tex2d.result().cast<Texture2d>();
        // texCube = async_texCube.result().cast<TextureCube>();

        auto scene_data = m_engine->asset_manager()->load(SID("assets/scenes/test_scene")).cast<SceneDataAsset>();

        scene = m_engine->game_manager()->make_scene(SID("test_scene"));
        m_engine->scene_manager()->build_scene(scene, scene_data->get_data());

        shader = m_engine->asset_manager()->load(SID("engine/shaders/canvas")).cast<Shader>();

        ShaderParamId p_clip_proj_view = shader->find_param_id(SID("ClipProjView"));
        ShaderParamId p_inverse_gamma  = shader->find_param_id(SID("InverseGamma"));

        ShaderParamBlock block(*shader, 0, SID("canvas"));
        block.set_var(p_clip_proj_view, Math3d::perspective(1.0f, 1.0f, 0.1f, 100000.f));
        block.set_var(p_inverse_gamma, 1.0f / 4.0f);
        // block.validate(Engine::instance()->gfx_driver(), Engine::instance()->gfx_ctx());

        Engine* engine = m_engine;

        // IoContext context;
        // context.add<ShaderManager*>(Engine::instance()->shader_manager());
        // IoYamlTree   tree;
        // IoStreamFile stream;
        // std::string  content;

        // WG_CHECKED(tree.create_tree());
        // WG_TREE_WRITE(context, tree, shader->get_reflection());
        // WG_CHECKED(tree.save_tree(content));
        // WG_CHECKED(Engine::instance()->file_system()->save_file("canvas.reflection.yaml", content));
        // WG_CHECKED(stream.open("canvas.reflection.bin", {FileOpenMode::Out, FileOpenMode::Binary}));
        // WG_ARCHIVE_WRITE(context, stream, shader->get_reflection());

        aux_draw  = std::make_unique<AuxDrawManager>();
        rdg_pool  = std::make_unique<RdgPool>(engine->gfx_driver());
        rdg_graph = std::make_unique<RdgGraph>(rdg_pool.get(), engine->gfx_driver(), engine->shader_manager(), engine->texture_manager());

        auto* ui_factory = engine->ui_manager()->get_factory();
        auto  menu_bar   = ui_factory->make_menu_bar();
        {
            auto menu_file = ui_factory->make_menu();
            menu_file->set_name("File");
            {
                auto g1           = ui_factory->make_menu_group();
                auto action_scene = ui_factory->make_menu_action();
                action_scene->set_name("Open Scene");
                g1->add_item(action_scene);

                auto g2             = ui_factory->make_menu_group();
                auto action_project = ui_factory->make_menu_action();
                action_project->set_name("Open Project");
                g2->add_item(action_project);

                menu_file->add_group(g1);
                menu_file->add_group(g2);
            }
            menu_bar->add_menu(menu_file);

            auto menu_help = ui_factory->make_menu();
            menu_help->set_name("Help");
            {
            }
            menu_bar->add_menu(menu_help);
        }

        auto scroll_area = ui_factory->make_scroll_area();
        scroll_area->set_name("console scroll");
        scroll_area->set_scroll(1.0f);
        for (int i = 0; i < 100; i++) {
            auto t1 = ui_factory->make_text();
            t1->set_text("[error] message");
            t1->set_text_color(Color::RED4f);
            scroll_area->add_element(t1);

            auto t2 = ui_factory->make_text();
            t2->set_text("[warning] message");
            t2->set_text_color(Color::YELLOW4f);
            scroll_area->add_element(t2);

            auto t3 = ui_factory->make_text();
            t3->set_text("[log] message");
            t3->set_text_color(Color::WHITE4f);
            scroll_area->add_element(t3);
        }

        auto text_input = ui_factory->make_text_input();

        auto text_input_popup = ui_factory->make_text_input_popup();
        text_input_popup->set_name("console input popup");
        text_input_popup->set_enabled(false);
        for (int i = 0; i < 10; i++) {
            auto s = ui_factory->make_selectable();
            s->set_name("eng.cmd.num" + std::to_string(i));
            s->set_on_click([s = s.get(), i = text_input.get()]() {
                i->set_text(s->get_name());
            });
            text_input_popup->add_element(s);
        }

        text_input->set_name("console input");
        text_input->set_text_flags({UiTextInputFlag::EnterForSubmit, UiTextInputFlag::CallbackAlways, UiTextInputFlag::EscapeToClear});
        text_input->set_popup(text_input_popup);
        text_input->set_hint_width(UiHintWidth{1.0f});
        text_input->set_on_text_input([i = text_input.get(), p = text_input_popup.get(), s = scroll_area.get(), ui_factory](const UiTextInputEvent& e) {
            p->set_enabled(!e.text.empty());

            if (e.type == UiTextInputEventType::Enter && !e.text.empty()) {
                auto t = ui_factory->make_text();
                t->set_text("[log] " + e.text);
                t->set_text_color(Color::WHITE4f);
                s->add_element(t);

                p->set_enabled(false);
                i->set_text("");
                s->set_scroll(1.0f);
            }
        });

        auto main_window = ui_factory->make_main_window();
        main_window->set_name("Main");
        main_window->set_menu_bar(menu_bar);
        engine->ui_manager()->provide_window(main_window);

        auto dock_area = ui_factory->make_layout_vertical();
        dock_area->add_element(scroll_area);
        dock_area->add_element(text_input);

        auto dock_window = ui_factory->make_dock_window();
        dock_window->set_name("Console");
        dock_window->set_window_flags({UiWindowFlag::NoBringToFrontOnFocus, UiWindowFlag::NoScrollbar});
        dock_window->set_area(dock_area);
        engine->ui_manager()->add_window(dock_window);

        WG_LOG_INFO("init");
        return WG_OK;
    }

    void on_debug_draw() override {
        WG_PROFILE_CPU_SCOPE(app, "TemplateGame::on_debug_draw");

        Engine* engine = m_engine;

        auto view = Math3d::look_at(Vec3f(0, 0, 0), Vec3f(0, 0, 1), Vec3f(0, 1, 0));
        auto proj = Math3d::perspective(Math::deg_to_rad(90.0f), 128.0f / 72.0f, 0.001f, 1000.0f);

        aux_draw->set_screen_size(Vec2f(1280, 720));
        aux_draw->set_font(font);
        aux_draw->draw_text_2d("frame id: " + std::to_string(engine->time()->get_iteration()), Vec2f(10, 30), 20.0f, Color::WHITE4f);
        aux_draw->draw_text_2d("scene: <" + scene->get_name().str() + ">", Vec2f(10, 10), 20.f, Color::YELLOW4f);
        aux_draw->draw_box(Vec3f(0, 0, 8.0f), Vec3f(2, 2, 2), Color::RED4f, Quatf::rotation(Vec3f::axis_y(), angle));
        aux_draw->draw_box(Vec3f(0, 0, 8.0f), Vec3f(2, 2, 2), Color::WHITE4f, Quatf::rotation(Vec3f::axis_y(), angle), false);

        Ref<Window> window = engine->window_manager()->get_primary_window();

        RdgTexture* color = rdg_graph->create_texture(GfxTextureDesc::make_2d(GfxFormat::RGBA8, 1280, 720, {GfxTexUsageFlag::ColorTarget, GfxTexUsageFlag::Sampling, GfxTexUsageFlag::Storage}), SIDDBG(""));
        RdgTexture* depth = rdg_graph->create_texture(GfxTextureDesc::make_2d(GfxFormat::DEPTH32F_STENCIL8, 1280, 720, {GfxTexUsageFlag::DepthStencilTarget}), SIDDBG(""));

        ShaderFuncs::fill(*rdg_graph, SIDDBG("clear"), color, Color::BLACK4f, engine->shader_table());
        aux_draw->flush(engine->time()->get_delta_time());
        aux_draw->render(*rdg_graph, color, depth, Rect2i{0, 0, 1280, 720}, 2.2f, proj * view, engine->shader_table(), engine->texture_manager());
        ShaderFuncs::blit(*rdg_graph, SIDDBG("blit"), window, color, engine->shader_table());

        rdg_graph->compile({});
        rdg_graph->execute({});
        rdg_graph->clear();

        rdg_pool->gc();

        angle += 0.01f;
    }

    Status on_shutdown() override {
        WG_PROFILE_CPU_SCOPE(app, "TemplateGame::on_shutdown");

        rdg_graph.reset();
        rdg_pool.reset();
        aux_draw.reset();
        font.reset();
        tex2d.reset();
        texCube.reset();
        shader.reset();
        mesh.reset();
        scene.reset();

        WG_CHECKED(GamePlugin::on_shutdown());
        return WG_OK;
    }

    Ref<Scene>                      scene;
    Ref<Texture2d>                  tex2d;
    Ref<TextureCube>                texCube;
    Ref<Mesh>                       mesh;
    Ref<Shader>                     shader;
    Ref<Font>                       font;
    std::unique_ptr<AuxDrawManager> aux_draw;
    std::unique_ptr<RdgPool>        rdg_pool;
    std::unique_ptr<RdgGraph>       rdg_graph;
    float                           angle = 0.0f;
};
