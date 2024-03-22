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

class TemplateApplication : public GameApplication {
public:
    ~TemplateApplication() override = default;

    Status on_register() override {
        GameApplication::on_register();

        WG_LOG_INFO("register");
        return StatusCode::Ok;
    }

    Status on_init() override {
        GameApplication::on_init();

        Engine::instance()->action_manager()->load("root://actions/actionmap_console.yml");
        Engine::instance()->action_manager()->load("root://actions/actionmap_camera_debug.yml");
        Engine::instance()->action_manager()->activate(SID("console"));
        Engine::instance()->action_manager()->activate(SID("camera_debug"));

        model = Engine::instance()->resource_manager()->load(SID("res://models/suzanne")).cast<Model>();

        auto scene_tree_packed = Engine::instance()->resource_manager()->load(SID("res://trees/test_scene")).cast<SceneTreePacked>();
        scene_tree             = scene_tree_packed->instantiate();
        scene                  = scene_tree->get_scene();

        Engine::instance()->scene_manager()->change(scene);

        class ApplicationLayer : public Layer {
        public:
            ApplicationLayer(TemplateApplication* game) : game(game) {}

            void on_iter() override {
                game->debug_draw();
            }

            TemplateApplication* game;
        };

        Engine::instance()->layer_stack()->attach(std::make_shared<ApplicationLayer>(this));

        auto shader  = Engine::instance()->resource_manager()->load(SID("res://shaders/test_shader")).cast<Shader>();
        auto variant = shader->create_variant({GfxVertAttribs{GfxVertAttrib::Pos3f}}, {"MESH_PASS_GBUFFER"});

        while (variant->status() == GfxShaderStatus::Compiling) {
            WG_LOG_INFO("waiting for " << variant->name() << " compilation...");
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }
        WG_LOG_INFO("status for " << variant->name() << " is " << magic_enum::enum_name(variant->status()));

        TextureManager* txm     = Engine::instance()->texture_manager();
        Ref<GfxTexture> tex_def = txm->get_gfx_default_texture_white();
        Ref<GfxSampler> smp_def = txm->get_gfx_default_sampler();

        GrcShaderClassBuilder builder;

        builder.set_name(SID("canvas"))
                .add_constant(SID("MAX_CANVAS_IMAGES"), 4)
                .add_struct(SID("Params"), 96)
                .add_field(SID("ClipProjView"), GrcShaderTypes::MAT4, TypedArray<float>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}))
                .add_field_array(SID("CustomData"), GrcShaderTypes::FLOAT, 4, Array({1, 2, 3, 4}))
                .add_field(SID("InverseGamma"), GrcShaderTypes::FLOAT, 1.0f / 2.2f)
                .add_field(SID("_pr_pad0"), GrcShaderTypes::FLOAT, 0.0f)
                .add_field(SID("_pr_pad1"), GrcShaderTypes::FLOAT, 0.0f)
                .add_field(SID("_pr_pad2"), GrcShaderTypes::FLOAT, 0.0f)
                .end_struct()
                .add_struct(SID("DrawCmd"), 80)
                .add_field(SID("Transform0"), GrcShaderTypes::VEC4)
                .add_field(SID("Transform1"), GrcShaderTypes::VEC4)
                .add_field(SID("Transform2"), GrcShaderTypes::VEC4)
                .add_field(SID("ClipRect"), GrcShaderTypes::VEC4)
                .add_field(SID("TextureIdx"), GrcShaderTypes::INT)
                .add_field(SID("_dc_pad0"), GrcShaderTypes::INT)
                .add_field(SID("_dc_pad1"), GrcShaderTypes::INT)
                .add_field(SID("_dc_pad2"), GrcShaderTypes::INT)
                .end_struct()
                .add_struct(SID("DrawCmds"), 0)
                .add_field_array(SID("Cmds"), SID("DrawCmd"))
                .end_struct()
                .add_space(SID("Default"), GrcShaderSpaceType::Default)
                .add_inline_uniform_buffer(SID("Params"), SID("Params"))
                //.add_storage_buffer(SID("DrawCmds"), SID("DrawCmds"))
                //.end_space()
                //.add_space(SID("Images"), GrcShaderSpaceType::Default)
                .add_texture_2d(SID("CanvasImage0"), tex_def, smp_def)
                .add_texture_2d(SID("CanvasImage1"), tex_def, smp_def)
                .add_texture_2d(SID("CanvasImage2"), tex_def, smp_def)
                .add_texture_2d(SID("CanvasImage3"), tex_def, smp_def)
                .end_space()
                .add_technique(SID("Default"))
                .add_pass(SID("Default"))
                .add_option(SID("OUT_COLOR"), {SID("SRGB"), SID("LINEAR")})
                .end_pass()
                .end_technique()
                .add_source(SID("canvas.vert"), GfxShaderModule::Vertex)
                .add_source(SID("canvas.frag"), GfxShaderModule::Fragment);

        std::shared_ptr<GrcShaderClass> shader_class;
        builder.finish(shader_class);

        GrcShaderParamId p_clip_proj_view = shader_class->get_param_id(SID("Params.ClipProjView"));
        GrcShaderParamId p_inverse_gamma  = shader_class->get_param_id(SID("Params.InverseGamma"));

        GrcShaderParamBlock block(*shader_class, 0);
        block.set_var(p_clip_proj_view, Math3d::perspective(1.0f, 1.0f, 0.1f, 100000.f));
        block.set_var(p_inverse_gamma, 1.0f / 4.0f);
        block.validate(Engine::instance()->gfx_driver(), Engine::instance()->gfx_ctx(), SID("test"));

        WG_LOG_INFO("init");
        return StatusCode::Ok;
    }

    void debug_draw() {
        WG_AUTO_PROFILE(app, "GameApplication::debug_draw");

        Engine*         engine           = Engine::instance();
        AuxDrawManager* aux_draw_manager = engine->aux_draw_manager();

        static float angle       = 0.0f;
        static int   frame_count = 0;

        angle += 0.001f;
        frame_count += 1;

        if (Random::next_float() < engine->time()->get_delta_time()) {
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

    Status on_shutdown() override {
        model.reset();
        mesh.reset();
        scene.reset();
        scene_tree.reset();

        GameApplication::on_shutdown();
        WG_LOG_INFO("shutdown");

        return StatusCode::Ok;
    }

    Ref<Scene>     scene;
    Ref<SceneTree> scene_tree;
    WeakRef<Mesh>  mesh;
    Ref<Model>     model;
};

int main(int argc, const char* const* argv) {
    TemplateApplication app;
    return app.run(argc, argv);
}