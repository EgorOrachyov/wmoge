// auto generated file

#pragma once

#include "grc/shader.hpp"

namespace wmoge {

    /// @brief Auto-generated reflection for 'aux_draw' shader
    struct ShaderAuxDraw {
        struct ParamBlockDefault {
            const Strid name = SID("default");

            const Strid   clipprojview_name = SID("ClipProjView");
            ShaderParamId clipprojview;

            const Strid   inversegamma_name = SID("InverseGamma");
            ShaderParamId inversegamma;

            const Strid   imagetexture_name = SID("ImageTexture");
            ShaderParamId imagetexture;

            void load_from(const Ref<Shader>& s);
        } pb_default;

        struct TechniqueDefault {
            const Strid name = SID("default");

            struct Options {
                const Strid               out_mode        = SID("OUT_MODE");
                const ShaderOptionVariant out_mode_srgb   = {SID("OUT_MODE"), SID("SRGB")};
                const ShaderOptionVariant out_mode_linear = {SID("OUT_MODE"), SID("LINEAR")};

            } options;

            struct PassSolid {
                const Strid name = SID("solid");

                operator Strid() const { return name; }

            } ps_solid;

            struct PassWire {
                const Strid name = SID("wire");

                operator Strid() const { return name; }

            } ps_wire;

            struct PassText {
                const Strid name = SID("text");

                operator Strid() const { return name; }

            } ps_text;

            operator Strid() const { return name; }

        } tq_default;

        Ref<Shader> shader;

        operator Shader*() const { return shader.get(); }

        Status load_from(Ref<Shader> s);
    };

    /// @brief Auto-generated reflection for 'blit' shader
    struct ShaderBlit {
        struct ParamBlockDefault {
            const Strid name = SID("default");

            const Strid   inversegamma_name = SID("InverseGamma");
            ShaderParamId inversegamma;

            const Strid   imagetexture_name = SID("ImageTexture");
            ShaderParamId imagetexture;

            void load_from(const Ref<Shader>& s);
        } pb_default;

        struct TechniqueDefault {
            const Strid name = SID("default");

            struct Options {
                const Strid               out_mode        = SID("OUT_MODE");
                const ShaderOptionVariant out_mode_none   = {SID("OUT_MODE"), SID("NONE")};
                const ShaderOptionVariant out_mode_srgb   = {SID("OUT_MODE"), SID("SRGB")};
                const ShaderOptionVariant out_mode_linear = {SID("OUT_MODE"), SID("LINEAR")};

            } options;

            struct PassDefault {
                const Strid name = SID("default");

                operator Strid() const { return name; }

            } ps_default;

            operator Strid() const { return name; }

        } tq_default;

        Ref<Shader> shader;

        operator Shader*() const { return shader.get(); }

        Status load_from(Ref<Shader> s);
    };

    /// @brief Auto-generated reflection for struct 'GpuCanvasDrawCmdData'
    struct GpuCanvasDrawCmdData {
        Vec4f Transform0;
        Vec4f Transform1;
        Vec4f Transform2;
        Vec4f ClipRect;
        int   TextureIdx;
        int   pad0;
        int   pad1;
        int   pad2;

        static Status reflect(class ShaderManager* sm);
    };

    /// @brief Auto-generated reflection for struct 'GpuCanvasDrawCmdsBuffer'
    struct GpuCanvasDrawCmdsBuffer {
        // GpuCanvasDrawCmdData DrawCmds[];

        static Status reflect(class ShaderManager* sm);
    };

    /// @brief Auto-generated reflection for 'canvas' shader
    struct ShaderCanvas {
        struct Constants {
            static constexpr int MAX_IMAGES = 4;
        };

        struct ParamBlockDefault {
            const Strid name = SID("default");

            const Strid   clipprojview_name = SID("ClipProjView");
            ShaderParamId clipprojview;

            const Strid   inversegamma_name = SID("InverseGamma");
            ShaderParamId inversegamma;

            const Strid   gpucanvasdrawcmdsbuffer_name = SID("GpuCanvasDrawCmdsBuffer");
            ShaderParamId gpucanvasdrawcmdsbuffer;

            void load_from(const Ref<Shader>& s);
        } pb_default;

        struct ParamBlockImagesBatch {
            const Strid name = SID("images_batch");

            const Strid   image0_name = SID("Image0");
            ShaderParamId image0;

            const Strid   image1_name = SID("Image1");
            ShaderParamId image1;

            const Strid   image2_name = SID("Image2");
            ShaderParamId image2;

            const Strid   image3_name = SID("Image3");
            ShaderParamId image3;

            void load_from(const Ref<Shader>& s);
        } pb_images_batch;

        struct TechniqueDefault {
            const Strid name = SID("default");

            struct Options {
                const Strid               out_mode        = SID("OUT_MODE");
                const ShaderOptionVariant out_mode_srgb   = {SID("OUT_MODE"), SID("SRGB")};
                const ShaderOptionVariant out_mode_linear = {SID("OUT_MODE"), SID("LINEAR")};

            } options;

            struct PassDefault {
                const Strid name = SID("default");

                operator Strid() const { return name; }

            } ps_default;

            operator Strid() const { return name; }

        } tq_default;

        Ref<Shader> shader;

        operator Shader*() const { return shader.get(); }

        Status load_from(Ref<Shader> s);
    };

    /// @brief Auto-generated reflection for 'fill' shader
    struct ShaderFill {
        struct Constants {
            static constexpr int GROUP_SIZE_DEFAULT = 8;
        };

        struct ParamBlockDefault {
            const Strid name = SID("default");

            const Strid   fillvalue_name = SID("FillValue");
            ShaderParamId fillvalue;

            const Strid   result_name = SID("Result");
            ShaderParamId result;

            void load_from(const Ref<Shader>& s);
        } pb_default;

        struct TechniqueDefault {
            const Strid name = SID("default");

            struct PassDefault {
                const Strid name = SID("default");

                operator Strid() const { return name; }

            } ps_default;

            operator Strid() const { return name; }

        } tq_default;

        Ref<Shader> shader;

        operator Shader*() const { return shader.get(); }

        Status load_from(Ref<Shader> s);
    };

    Status reflect_shader_types(class ShaderManager* sm);

}// namespace wmoge