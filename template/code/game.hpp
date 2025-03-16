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

        auto atlas = m_engine->asset_manager()->load(SID("editor/icons/atlas")).cast<IconAtlas>();
        auto icon  = atlas->try_find_icon(SID("general_redo")).value();

        auto editor_style = m_engine->asset_manager()->load(SID("editor/styles/dark")).cast<UiStyle>();

        auto button_save      = make_ref<UiButton>();
        button_save->label    = "save style";
        button_save->icon     = icon;
        button_save->on_click = [e = m_engine]() {
            WG_LOG_INFO("saved!");

            IoYamlTree tree;
            IoContext  ctx;

            tree.create_tree();

            auto s = e->ui_manager()->get_style();
            s->write_to_tree(ctx, tree);

            std::string str;
            tree.save_tree(str);

            e->file_system()->save_file("style.yml", str);
        };

        auto button_load      = make_ref<UiButton>();
        button_load->label    = "load style";
        button_load->icon     = icon;
        button_load->on_click = [e = m_engine, editor_style]() {
            WG_LOG_INFO("loaded!");

            IoYamlTree tree;
            IoContext  ctx;

            tree.parse_file(e->file_system(), "style.yml");

            auto s = make_ref<UiStyle>();
            s->set_id(SID("loaded_style"));
            s->read_from_tree(ctx, tree);

            e->ui_manager()->set_style(s);
        };

        auto panel                 = make_ref<UiStackPanel>();
        panel->children.add_slot() = button_save;
        panel->children.add_slot() = button_load;

        auto window   = make_ref<UiMainWindow>();
        window->title = "Wmoge Editor";
        window->panel = panel;

        m_engine->ui_manager()->set_main_window(window);
        m_engine->ui_manager()->set_style(editor_style);

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
