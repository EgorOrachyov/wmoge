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

#include "io/stream_file.hpp"
#include "io/tree_yaml.hpp"
#include "render/shader_funcs.hpp"

#include "assimp_plugin.hpp"
#include "freetype_plugin.hpp"
#include "runtime_plugin.hpp"

namespace wmoge {

    struct TestStruct {
        WG_RTTI_STRUCT(TestStruct);

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
        WG_RTTI_CLASS(TestClass, RttiObject);

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
    TemplateApplication(GameApplicationConfig& config) : GameApplication(config) {
    }

    Status on_register() override {
        GameApplication::on_register();

        WG_LOG_INFO("register");
        return StatusCode::Ok;
    }

    Status on_init() override {
        WG_PROFILE_CPU_SCOPE(app, "TemplateApplication::on_init");

        GameApplication::on_init();

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

        WG_LOG_INFO("init");
        return StatusCode::Ok;
    }

    void on_debug_draw() override {
        WG_PROFILE_CPU_SCOPE(app, "TemplateApplication::on_debug_draw");

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
        WG_PROFILE_CPU_SCOPE(app, "TemplateApplication::on_shutdown");

        rdg_graph.reset();
        rdg_pool.reset();
        aux_draw.reset();
        font.reset();
        tex2d.reset();
        texCube.reset();
        shader.reset();
        mesh.reset();
        scene.reset();

        GameApplication::on_shutdown();
        WG_LOG_INFO("shutdown");

        return StatusCode::Ok;
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

int main(int argc, const char* const* argv) {
    IocContainer    ioc_containter;
    CmdLineOptions  options("template-app", "wmoge engine template app for testing");
    CmdLineHookList hooks;

    ApplicationSignals app_signlas;

    EngineCmdLineHooks::uuid_gen(options, hooks);
    EngineCmdLineHooks::root_remap(options, hooks, &ioc_containter);
    EngineCmdLineHooks::engine(options, hooks, &ioc_containter);
    EngineCmdLineHooks::logs(options, hooks, &ioc_containter);
    EngineCmdLineHooks::profiler(options, hooks, &ioc_containter, &app_signlas);

    ApplicationCmdLine cmd_line;
    cmd_line.options = &options;
    cmd_line.hooks   = &hooks;
    cmd_line.line    = CmdLineUtil::to_string(argc, argv);
    cmd_line.args    = CmdLineUtil::to_vector(argc, argv);

    ApplicationConfig app_config;
    app_config.name     = "template game";
    app_config.ioc      = &ioc_containter;
    app_config.signals  = &app_signlas;
    app_config.cmd_line = &cmd_line;

    EngineApplicationConfig engine_app_config;
    engine_app_config.app_config = &app_config;
    engine_app_config.plugins    = {std::make_shared<RuntimePlugin>(), std::make_shared<AssimpPlugin>(), std::make_shared<FreetypePlugin>()};

    GameApplicationConfig game_app_config;
    game_app_config.app_config = &engine_app_config;
    game_app_config.game_info  = "template game application";

    TemplateApplication app(game_app_config);
    return app.run();
}