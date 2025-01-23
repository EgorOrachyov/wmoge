// auto generated file

#pragma once

#include "interop.hpp"

#include "grc/shader_manager.hpp"
#include "grc/shader_reflection.hpp"

namespace wmoge {

    void ShaderAuxDraw::ParamBlockDefault::load_from(const Ref<Shader>& shader) {
        clipprojview = shader->find_param_id(clipprojview_name);
        inversegamma = shader->find_param_id(inversegamma_name);
        imagetexture = shader->find_param_id(imagetexture_name);
    }

    Status ShaderAuxDraw::load_from(Ref<Shader> s) {
        shader = std::move(s);
        pb_default.load_from(shader);
        return WG_OK;
    }

    void ShaderBlit::ParamBlockDefault::load_from(const Ref<Shader>& shader) {
        inversegamma = shader->find_param_id(inversegamma_name);
        imagetexture = shader->find_param_id(imagetexture_name);
    }

    Status ShaderBlit::load_from(Ref<Shader> s) {
        shader = std::move(s);
        pb_default.load_from(shader);
        return WG_OK;
    }

    Status GpuCanvasDrawCmdData::reflect(class ShaderManager* sm) {
        ShaderStructRegister r(Strid("GpuCanvasDrawCmdData"), 80, sm);
        r.add_field(Strid("Transform0"), Strid("vec4"));
        r.add_field(Strid("Transform1"), Strid("vec4"));
        r.add_field(Strid("Transform2"), Strid("vec4"));
        r.add_field(Strid("ClipRect"), Strid("vec4"));
        r.add_field(Strid("TextureIdx"), Strid("int"));
        r.add_field(Strid("pad0"), Strid("int"));
        r.add_field(Strid("pad1"), Strid("int"));
        r.add_field(Strid("pad2"), Strid("int"));
        return r.finish();
    }

    Status GpuCanvasDrawCmdsBuffer::reflect(class ShaderManager* sm) {
        ShaderStructRegister r(Strid("GpuCanvasDrawCmdsBuffer"), 0, sm);
        r.add_field_array(Strid("DrawCmds"), Strid("GpuCanvasDrawCmdData"));
        return r.finish();
    }

    void ShaderCanvas::ParamBlockDefault::load_from(const Ref<Shader>& shader) {
        clipprojview            = shader->find_param_id(clipprojview_name);
        inversegamma            = shader->find_param_id(inversegamma_name);
        gpucanvasdrawcmdsbuffer = shader->find_param_id(gpucanvasdrawcmdsbuffer_name);
    }

    void ShaderCanvas::ParamBlockImagesBatch::load_from(const Ref<Shader>& shader) {
        image0 = shader->find_param_id(image0_name);
        image1 = shader->find_param_id(image1_name);
        image2 = shader->find_param_id(image2_name);
        image3 = shader->find_param_id(image3_name);
    }

    Status ShaderCanvas::load_from(Ref<Shader> s) {
        shader = std::move(s);
        pb_default.load_from(shader);
        pb_images_batch.load_from(shader);
        return WG_OK;
    }

    void ShaderFill::ParamBlockDefault::load_from(const Ref<Shader>& shader) {
        fillvalue = shader->find_param_id(fillvalue_name);
        result    = shader->find_param_id(result_name);
    }

    Status ShaderFill::load_from(Ref<Shader> s) {
        shader = std::move(s);
        pb_default.load_from(shader);
        return WG_OK;
    }

    Status reflect_shader_types(class ShaderManager* sm) {
        WG_CHECKED(GpuCanvasDrawCmdData::reflect(sm));
        WG_CHECKED(GpuCanvasDrawCmdsBuffer::reflect(sm));
        return WG_OK;
    };

}// namespace wmoge