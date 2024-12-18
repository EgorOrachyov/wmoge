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

        auto async_mesh  = m_engine->asset_manager()->load_async(SID("assets/mesh/suzanne"));
        auto async_tex2d = m_engine->asset_manager()->load_async(SID("assets/textures/dirt_mask"));
        // auto async_texCube = m_engine->asset_manager()->load_async(SID("assets/textures/skybox"));

        async_mesh.wait_completed();
        async_tex2d.wait_completed();
        // async_texCube.wait_completed();

        mesh  = async_mesh.result().cast<Mesh>();
        tex2d = async_tex2d.result().cast<Texture2d>();
        // texCube = async_texCube.result().cast<TextureCube>();

        auto scene_data = m_engine->asset_manager()->load(SID("assets/scenes/test_scene")).cast<SceneDataAsset>();

        auto s = m_engine->game_manager()->make_scene(SID("test_scene"));
        m_engine->scene_manager()->build_scene(s, scene_data->get_data());

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

        WG_LOG_INFO("init");
        return StatusCode::Ok;
    }

    void debug_draw() {
        WG_PROFILE_CPU_SCOPE(app, "TemplateApplication::debug_draw");

        Engine*        engine         = m_engine;
        ShaderManager* shader_manager = engine->shader_manager();

        ShaderPermutation permutation;
        permutation.technique_idx = 0;
        permutation.pass_idx      = 0;

        permutation.options.reset();
        permutation.options.set(0);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f};
        auto p1                  = shader_manager->get_or_create_program(shader.get(), GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos2f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f};
        auto p2                  = shader_manager->get_or_create_program(shader.get(), GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Uv12f, GfxVertAttrib::Col04f};
        auto p3                  = shader_manager->get_or_create_program(shader.get(), GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f, GfxVertAttrib::Col14f};
        auto p4                  = shader_manager->get_or_create_program(shader.get(), GfxShaderPlatform::VulkanWindows, permutation);

        permutation.options.reset();
        permutation.options.set(1);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f};
        auto p5                  = shader_manager->get_or_create_program(shader.get(), GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos2f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f};
        auto p6                  = shader_manager->get_or_create_program(shader.get(), GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Uv12f, GfxVertAttrib::Col04f};
        auto p7                  = shader_manager->get_or_create_program(shader.get(), GfxShaderPlatform::VulkanWindows, permutation);

        permutation.vert_attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Uv02f, GfxVertAttrib::Col04f, GfxVertAttrib::Col14f};
        auto p8                  = shader_manager->get_or_create_program(shader.get(), GfxShaderPlatform::VulkanWindows, permutation);

        static float angle       = 0.0f;
        static int   frame_count = 0;

        angle += 0.001f;
        frame_count += 1;
    }

    Status on_shutdown() override {
        WG_PROFILE_CPU_SCOPE(app, "TemplateApplication::on_shutdown");

        tex2d.reset();
        texCube.reset();
        shader.reset();
        mesh.reset();
        scene.reset();

        GameApplication::on_shutdown();
        WG_LOG_INFO("shutdown");

        return StatusCode::Ok;
    }

    Ref<Scene>       scene;
    Ref<Texture2d>   tex2d;
    Ref<TextureCube> texCube;
    Ref<Mesh>        mesh;
    Ref<Shader>      shader;
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

    GameApplicationConfig app_config;
    app_config.name     = "test app";
    app_config.ioc      = &ioc_containter;
    app_config.signals  = &app_signlas;
    app_config.cmd_line = &cmd_line;
    app_config.plugins  = {std::make_shared<RuntimePlugin>(), std::make_shared<AssimpPlugin>(), std::make_shared<FreetypePlugin>()};

    TemplateApplication app(app_config);
    return app.run();
}