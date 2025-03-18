// auto generated file

#pragma once

#include "interop.hpp"

#include "grc/shader_manager.hpp"
#include "grc/shader_reflection.hpp"

namespace wmoge {

    ShaderAuxDraw::ParamBlockDefault::ParamBlockDefault(const ShaderAuxDraw* shader_cls, RdgResourceId id)
        : RdgParamBlock(shader_cls->shader.get(), 0, id, SIDDBG("default")) {
        m_shader_cls = shader_cls;
    }

    void ShaderAuxDraw::ParamBlockDefault::pack() {
        m_ptr->set_var(m_shader_cls->pb_default.clipprojview, vars.clipprojview);
        m_ptr->set_var(m_shader_cls->pb_default.inversegamma, vars.inversegamma);
        m_ptr->set_var(m_shader_cls->pb_default.imagetexture, vars.imagetexture->get_texture_ref());
        m_ptr->set_var(m_shader_cls->pb_default.imagetexture, vars.imagetexture_sampler);
    }

    void ShaderAuxDraw::ParamBlockMetaDefault::load_from(const Ref<Shader>& shader) {
        clipprojview = shader->find_param_id(clipprojview_name);
        inversegamma = shader->find_param_id(inversegamma_name);
        imagetexture = shader->find_param_id(imagetexture_name);
    }

    Status ShaderAuxDraw::load_from(Ref<Shader> s) {
        shader = std::move(s);
        pb_default.load_from(shader);
        return WG_OK;
    }

    ShaderBlit::ParamBlockDefault::ParamBlockDefault(const ShaderBlit* shader_cls, RdgResourceId id)
        : RdgParamBlock(shader_cls->shader.get(), 0, id, SIDDBG("default")) {
        m_shader_cls = shader_cls;
    }

    void ShaderBlit::ParamBlockDefault::pack() {
        m_ptr->set_var(m_shader_cls->pb_default.gamma, vars.gamma);
        m_ptr->set_var(m_shader_cls->pb_default.inversegamma, vars.inversegamma);
        m_ptr->set_var(m_shader_cls->pb_default.imagetexture, vars.imagetexture->get_texture_ref());
        m_ptr->set_var(m_shader_cls->pb_default.imagetexture, vars.imagetexture_sampler);
    }

    void ShaderBlit::ParamBlockMetaDefault::load_from(const Ref<Shader>& shader) {
        gamma        = shader->find_param_id(gamma_name);
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

    ShaderCanvas::ParamBlockDefault::ParamBlockDefault(const ShaderCanvas* shader_cls, RdgResourceId id)
        : RdgParamBlock(shader_cls->shader.get(), 0, id, SIDDBG("default")) {
        m_shader_cls = shader_cls;
    }

    void ShaderCanvas::ParamBlockDefault::pack() {
        m_ptr->set_var(m_shader_cls->pb_default.clipprojview, vars.clipprojview);
        m_ptr->set_var(m_shader_cls->pb_default.inversegamma, vars.inversegamma);
        m_ptr->set_var(m_shader_cls->pb_default.gpucanvasdrawcmdsbuffer, vars.gpucanvasdrawcmdsbuffer->get_buffer_ref());
    }

    void ShaderCanvas::ParamBlockMetaDefault::load_from(const Ref<Shader>& shader) {
        clipprojview            = shader->find_param_id(clipprojview_name);
        inversegamma            = shader->find_param_id(inversegamma_name);
        gpucanvasdrawcmdsbuffer = shader->find_param_id(gpucanvasdrawcmdsbuffer_name);
    }

    ShaderCanvas::ParamBlockImagesBatch::ParamBlockImagesBatch(const ShaderCanvas* shader_cls, RdgResourceId id)
        : RdgParamBlock(shader_cls->shader.get(), 1, id, SIDDBG("images_batch")) {
        m_shader_cls = shader_cls;
    }

    void ShaderCanvas::ParamBlockImagesBatch::pack() {
        m_ptr->set_var(m_shader_cls->pb_images_batch.image0, vars.image0->get_texture_ref());
        m_ptr->set_var(m_shader_cls->pb_images_batch.image0, vars.image0_sampler);
        m_ptr->set_var(m_shader_cls->pb_images_batch.image1, vars.image1->get_texture_ref());
        m_ptr->set_var(m_shader_cls->pb_images_batch.image1, vars.image1_sampler);
        m_ptr->set_var(m_shader_cls->pb_images_batch.image2, vars.image2->get_texture_ref());
        m_ptr->set_var(m_shader_cls->pb_images_batch.image2, vars.image2_sampler);
        m_ptr->set_var(m_shader_cls->pb_images_batch.image3, vars.image3->get_texture_ref());
        m_ptr->set_var(m_shader_cls->pb_images_batch.image3, vars.image3_sampler);
    }

    void ShaderCanvas::ParamBlockMetaImagesBatch::load_from(const Ref<Shader>& shader) {
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

    ShaderFill::ParamBlockDefault::ParamBlockDefault(const ShaderFill* shader_cls, RdgResourceId id)
        : RdgParamBlock(shader_cls->shader.get(), 0, id, SIDDBG("default")) {
        m_shader_cls = shader_cls;
    }

    void ShaderFill::ParamBlockDefault::pack() {
        m_ptr->set_var(m_shader_cls->pb_default.fillvalue, vars.fillvalue);
        m_ptr->set_var(m_shader_cls->pb_default.result, vars.result->get_texture_ref());
    }

    void ShaderFill::ParamBlockMetaDefault::load_from(const Ref<Shader>& shader) {
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