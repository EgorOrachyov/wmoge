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

namespace wmoge {

    class TestWindowMain : public RttiObject {
    public:
        WG_RTTI_CLASS(TestWindowMain, RttiObject);

        void on_event() {
            WG_LOG_INFO("on event");
        }

    protected:
        std::string m_text  = "Do some work";
        int         m_value = 0;
    };

    WG_RTTI_CLASS_BEGIN(TestWindowMain) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_text, {});
        WG_RTTI_FIELD(m_value, {});
        WG_RTTI_METHOD(on_event, {}, {});
    }
    WG_RTTI_END;

}// namespace wmoge

using namespace wmoge;

class TemplateGame : public GamePlugin {
public:
    IocContainer* m_ioc;

    TemplateGame() {
    }

    Status on_register(IocContainer* ioc) {
        WG_CHECKED(GamePlugin::on_register(ioc));
        rtti_type<TestWindowMain>();
        m_ioc = ioc;
        return WG_OK;
    }

    Status on_init() override {
        WG_PROFILE_CPU_SCOPE(app, "TemplateGame::on_init");

        WG_CHECKED(GamePlugin::on_init());

        // auto async_font = m_engine->asset_manager()->load("assets/fonts/consolas.font");
        // auto async_mesh  = m_engine->asset_manager()->load_async(SID("assets/mesh/suzanne"));
        // auto async_tex2d = m_engine->asset_manager()->load_async(SID("assets/textures/dirt_mask"));
        // auto async_texCube = m_engine->asset_manager()->load_async(SID("assets/textures/skybox"));

        // async_font.wait_completed();
        // async_mesh.wait_completed();
        // async_tex2d.wait_completed();
        // async_texCube.wait_completed();

        // font = async_font.result().cast<Font>();
        // mesh  = async_mesh.result().cast<Mesh>();
        // tex2d = async_tex2d.result().cast<Texture2d>();
        // texCube = async_texCube.result().cast<TextureCube>();

        // auto scene_data = m_engine->asset_manager()->load(SID("assets/scenes/test_scene")).cast<SceneDataAsset>();

        scene = m_engine->game_manager()->make_scene(SID("test_scene"));
        // m_engine->scene_manager()->build_scene(scene, scene_data->get_data());

        Engine* engine = m_engine;

        aux_draw  = std::make_unique<AuxDrawManager>();
        rdg_pool  = std::make_unique<RdgPool>(engine->gfx_driver());
        rdg_graph = std::make_unique<RdgGraph>(rdg_pool.get(), engine->gfx_driver(), engine->shader_manager(), engine->texture_manager());

        // auto atlas = m_engine->asset_manager()->load(SID("editor/icons/atlas")).cast<IconAtlas>();
        // auto icon  = atlas->try_find_icon(SID("general_redo")).value();

        auto style  = m_engine->asset_manager()->load_wait("editor/styles/dark.style").cast<UiStyle>();
        auto markup = m_engine->asset_manager()->load_wait("editor/views/window_main.uixml").cast<UiMarkup>();

        Ref<UiElement>      window   = markup->make_elements();
        Ref<TestWindowMain> bindable = make_ref<TestWindowMain>();

        UiBinder binder(window, bindable);
        WG_CHECKED(binder.bind());

        m_engine->ui_manager()->set_main_window(window.cast<UiMainWindow>());
        m_engine->ui_manager()->set_style(style);

        WG_LOG_INFO("init");
        return WG_OK;
    }

    void on_debug_draw() override {
        WG_PROFILE_CPU_SCOPE(app, "TemplateGame::on_debug_draw");

        Engine* engine = m_engine;

        Ref<Window> window = engine->window_manager()->get_primary_window();
        auto        size   = window->fbo_size();

        if (size[0] <= 0 || size[1] <= 0) {
            return;
        }

        auto view = Math3d::look_at(Vec3f(0, 0, 0), Vec3f(0, 0, 1), Vec3f(0, 1, 0));
        auto proj = Math3d::perspective(Math::deg_to_rad(90.0f), float(size[0]) / float(size[1]), 0.001f, 1000.0f);

        // aux_draw->set_screen_size(Vec2f(float(size[0]), float(size[1])));
        // aux_draw->set_font(font);
        // aux_draw->draw_text_2d("frame id: " + std::to_string(engine->time()->get_iteration()), Vec2f(10, 30), 20.0f, Color::WHITE4f);
        // aux_draw->draw_text_2d("scene: <" + scene->get_name().str() + ">", Vec2f(10, 10), 20.f, Color::YELLOW4f);
        // aux_draw->draw_box(Vec3f(0, 0, 8.0f), Vec3f(2, 2, 2), Color::RED4f, Quatf::rotation(Vec3f::axis_y(), angle));
        // aux_draw->draw_box(Vec3f(0, 0, 8.0f), Vec3f(2, 2, 2), Color::WHITE4f, Quatf::rotation(Vec3f::axis_y(), angle), false);

        RdgTexture* color = rdg_graph->create_texture(GfxTextureDesc::make_2d(GfxFormat::RGBA8, size[0], size[1], {GfxTexUsageFlag::ColorTarget, GfxTexUsageFlag::Sampling, GfxTexUsageFlag::Storage}), SIDDBG(""));
        RdgTexture* depth = rdg_graph->create_texture(GfxTextureDesc::make_2d(GfxFormat::DEPTH32F_STENCIL8, size[0], size[1], {GfxTexUsageFlag::DepthStencilTarget}), SIDDBG(""));

        ShaderFuncs::fill(*rdg_graph, SIDDBG("clear"), color, Color::BLACK4f, engine->shader_table());
        // aux_draw->flush(engine->time()->get_delta_time());
        // aux_draw->render(*rdg_graph, color, depth, Rect2i{0, 0, size[0], size[1]}, 1.f, proj * view, engine->shader_table(), engine->texture_manager());

        engine->ui_manager()->update(engine->time()->get_iteration());
        engine->ui_manager()->render(*rdg_graph, color);

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
        mesh.reset();
        scene.reset();

        WG_CHECKED(GamePlugin::on_shutdown());
        return WG_OK;
    }

    Ref<Scene>                      scene;
    Ref<Texture2d>                  tex2d;
    Ref<TextureCube>                texCube;
    Ref<Mesh>                       mesh;
    Ref<Font>                       font;
    std::unique_ptr<AuxDrawManager> aux_draw;
    std::unique_ptr<RdgPool>        rdg_pool;
    std::unique_ptr<RdgGraph>       rdg_graph;
    float                           angle = 0.0f;
};
