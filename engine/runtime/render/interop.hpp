// auto generated file

#pragma once

#include "grc/shader.hpp"
#include "grc/shader_param_block.hpp"
#include "rdg/rdg_resources.hpp"

namespace wmoge {

    /// @brief Auto-generated reflection for 'aux_draw' shader
    struct ShaderAuxDraw {
        class ParamBlockDefault : public RdgParamBlock {
        public:
            ParamBlockDefault(const ShaderAuxDraw* shader_cls, RdgResourceId id);

            struct Vars {
                RdgTexture*     clipprojview;
                float           inversegamma;
                RdgTexture*     imagetexture;
                Ref<GfxSampler> imagetexture_sampler;
            } vars;

            void pack() override;

        private:
            const ShaderAuxDraw* m_shader_cls;
        };

        struct ParamBlockMetaDefault {
            const Strid   name              = SID("default");
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
                const Strid pass_name      = SID("solid");
                const Strid technique_name = SID("default");

            } ps_solid;

            struct PassWire {
                const Strid pass_name      = SID("wire");
                const Strid technique_name = SID("default");

            } ps_wire;

            struct PassText {
                const Strid pass_name      = SID("text");
                const Strid technique_name = SID("default");

            } ps_text;

        } tq_default;

        Ref<Shader> shader;

        Status load_from(Ref<Shader> s);
    };

    /// @brief Auto-generated reflection for 'blit' shader
    struct ShaderBlit {
        class ParamBlockDefault : public RdgParamBlock {
        public:
            ParamBlockDefault(const ShaderBlit* shader_cls, RdgResourceId id);

            struct Vars {
                float           inversegamma;
                RdgTexture*     imagetexture;
                Ref<GfxSampler> imagetexture_sampler;
            } vars;

            void pack() override;

        private:
            const ShaderBlit* m_shader_cls;
        };

        struct ParamBlockMetaDefault {
            const Strid   name              = SID("default");
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
                const Strid pass_name      = SID("default");
                const Strid technique_name = SID("default");

            } ps_default;

        } tq_default;

        Ref<Shader> shader;

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

        class ParamBlockDefault : public RdgParamBlock {
        public:
            ParamBlockDefault(const ShaderCanvas* shader_cls, RdgResourceId id);

            struct Vars {
                RdgTexture*       clipprojview;
                float             inversegamma;
                RdgStorageBuffer* gpucanvasdrawcmdsbuffer;
            } vars;

            void pack() override;

        private:
            const ShaderCanvas* m_shader_cls;
        };

        struct ParamBlockMetaDefault {
            const Strid   name              = SID("default");
            const Strid   clipprojview_name = SID("ClipProjView");
            ShaderParamId clipprojview;
            const Strid   inversegamma_name = SID("InverseGamma");
            ShaderParamId inversegamma;
            const Strid   gpucanvasdrawcmdsbuffer_name = SID("GpuCanvasDrawCmdsBuffer");
            ShaderParamId gpucanvasdrawcmdsbuffer;

            void load_from(const Ref<Shader>& s);
        } pb_default;

        class ParamBlockImagesBatch : public RdgParamBlock {
        public:
            ParamBlockImagesBatch(const ShaderCanvas* shader_cls, RdgResourceId id);

            struct Vars {
                RdgTexture*     image0;
                Ref<GfxSampler> image0_sampler;
                RdgTexture*     image1;
                Ref<GfxSampler> image1_sampler;
                RdgTexture*     image2;
                Ref<GfxSampler> image2_sampler;
                RdgTexture*     image3;
                Ref<GfxSampler> image3_sampler;
            } vars;

            void pack() override;

        private:
            const ShaderCanvas* m_shader_cls;
        };

        struct ParamBlockMetaImagesBatch {
            const Strid   name        = SID("images_batch");
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
                const Strid pass_name      = SID("default");
                const Strid technique_name = SID("default");

            } ps_default;

        } tq_default;

        Ref<Shader> shader;

        Status load_from(Ref<Shader> s);
    };

    /// @brief Auto-generated reflection for 'fill' shader
    struct ShaderFill {
        struct Constants {
            static constexpr int GROUP_SIZE_DEFAULT = 8;
        };

        class ParamBlockDefault : public RdgParamBlock {
        public:
            ParamBlockDefault(const ShaderFill* shader_cls, RdgResourceId id);

            struct Vars {
                Vec4f       fillvalue;
                RdgTexture* result;
            } vars;

            void pack() override;

        private:
            const ShaderFill* m_shader_cls;
        };

        struct ParamBlockMetaDefault {
            const Strid   name           = SID("default");
            const Strid   fillvalue_name = SID("FillValue");
            ShaderParamId fillvalue;
            const Strid   result_name = SID("Result");
            ShaderParamId result;

            void load_from(const Ref<Shader>& s);
        } pb_default;

        struct TechniqueDefault {
            const Strid name = SID("default");

            struct PassDefault {
                const Strid pass_name      = SID("default");
                const Strid technique_name = SID("default");

            } ps_default;

        } tq_default;

        Ref<Shader> shader;

        Status load_from(Ref<Shader> s);
    };

    Status reflect_shader_types(class ShaderManager* sm);

}// namespace wmoge