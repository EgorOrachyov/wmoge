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

#include "vk_shader.hpp"

#include "core/timer.hpp"
#include "debug/profiler.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "io/archive.hpp"
#include "io/archive_memory.hpp"
#include "io/enum.hpp"

namespace wmoge {

    WG_IO_BEGIN(VKShaderBinary)
    WG_IO_FIELD(spirvs)
    WG_IO_FIELD(layouts)
    WG_IO_FIELD(reflection)
    WG_IO_END(VKShaderBinary)

    VKShader::VKShader(std::string vertex, std::string fragment, const GfxDescSetLayouts& layouts, const Strid& name, class VKDriver& driver)
        : VKResource<GfxShader>(driver) {

        m_name = name;
        m_sources.push_back(std::move(vertex));
        m_sources.push_back(std::move(fragment));
        m_set_layouts = layouts;
    }
    VKShader::VKShader(std::string compute, const GfxDescSetLayouts& layouts, const Strid& name, VKDriver& driver)
        : VKResource<GfxShader>(driver) {

        m_name = name;
        m_sources.push_back(std::move(compute));
        m_set_layouts = layouts;
    }
    VKShader::VKShader(Ref<Data> byte_code, const Strid& name, class VKDriver& driver)
        : VKResource<GfxShader>(driver) {

        m_name      = name;
        m_byte_code = std::move(byte_code);
    }

    VKShader::~VKShader() {
        WG_AUTO_PROFILE_VULKAN("VKShader::~VKShader");

        for (auto& module : m_modules) {
            vkDestroyShaderModule(m_driver.device(), module, nullptr);
        }
        if (m_layout) {
            vkDestroyPipelineLayout(m_driver.device(), m_layout, nullptr);
        }
    }

    GfxShaderStatus VKShader::status() const {
        return m_status.load();
    }
    std::string VKShader::message() const {
        return status() != GfxShaderStatus::Compiling ? m_message : std::string();
    }
    std::optional<const GfxShaderReflection*> VKShader::reflection() const {
        if (status() == GfxShaderStatus::Compiled) {
            return &m_reflection;
        }
        return std::nullopt;
    }
    Ref<Data> VKShader::byte_code() const {
        return status() == GfxShaderStatus::Compiled ? m_byte_code : Ref<Data>();
    }
    void VKShader::compile_from_source() {
        WG_AUTO_PROFILE_VULKAN("VKShader::compile_from_source");

        const bool compile_vert_frag = m_sources.size() == 2;
        const bool compile_compute   = m_sources.size() == 1;

        assert(compile_vert_frag || compile_compute);

        Timer timer;
        timer.start();

        const auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

        auto compile_single_shader = [&](EShLanguage language, glslang::TShader& shader, const std::string& source) {
            const char* shader_sources[1] = {source.data()};
            const int   shader_lengths[1] = {int(source.size())};

            shader.setStringsWithLengths(shader_sources, shader_lengths, 1);

            int                               version        = 100;
            glslang::EShClient                client         = glslang::EShClientVulkan;
            glslang::EShTargetClientVersion   client_version = glslang::EShTargetVulkan_1_0;
            glslang::EShTargetLanguageVersion target_version = glslang::EShTargetSpv_1_0;

            shader.setEnvInput(glslang::EShSourceGlsl, language, client, version);
            shader.setEnvClient(glslang::EShClientVulkan, client_version);
            shader.setEnvTarget(glslang::EShTargetSpv, target_version);
            shader.setEntryPoint("main");

            TBuiltInResource built_in_res{};
            built_in_res.maxLights                                 = 32;
            built_in_res.maxClipPlanes                             = 6;
            built_in_res.maxTextureUnits                           = 32;
            built_in_res.maxTextureCoords                          = 32;
            built_in_res.maxVertexAttribs                          = 64;
            built_in_res.maxVertexUniformComponents                = 4096;
            built_in_res.maxVaryingFloats                          = 64;
            built_in_res.maxVertexTextureImageUnits                = 32;
            built_in_res.maxCombinedTextureImageUnits              = 80;
            built_in_res.maxTextureImageUnits                      = 32;
            built_in_res.maxFragmentUniformComponents              = 4096;
            built_in_res.maxDrawBuffers                            = 32;
            built_in_res.maxVertexUniformVectors                   = 128;
            built_in_res.maxVaryingVectors                         = 8;
            built_in_res.maxFragmentUniformVectors                 = 16;
            built_in_res.maxVertexOutputVectors                    = 16;
            built_in_res.maxFragmentInputVectors                   = 15;
            built_in_res.minProgramTexelOffset                     = -8;
            built_in_res.maxProgramTexelOffset                     = 7;
            built_in_res.maxClipDistances                          = 8;
            built_in_res.maxComputeWorkGroupCountX                 = 65535;
            built_in_res.maxComputeWorkGroupCountY                 = 65535;
            built_in_res.maxComputeWorkGroupCountZ                 = 65535;
            built_in_res.maxComputeWorkGroupSizeX                  = 1024;
            built_in_res.maxComputeWorkGroupSizeY                  = 1024;
            built_in_res.maxComputeWorkGroupSizeZ                  = 64;
            built_in_res.maxComputeUniformComponents               = 1024;
            built_in_res.maxComputeTextureImageUnits               = 16;
            built_in_res.maxComputeImageUniforms                   = 8;
            built_in_res.maxComputeAtomicCounters                  = 8;
            built_in_res.maxComputeAtomicCounterBuffers            = 1;
            built_in_res.maxVaryingComponents                      = 60;
            built_in_res.maxVertexOutputComponents                 = 64;
            built_in_res.maxGeometryInputComponents                = 64;
            built_in_res.maxGeometryOutputComponents               = 128;
            built_in_res.maxFragmentInputComponents                = 128;
            built_in_res.maxImageUnits                             = 8;
            built_in_res.maxCombinedImageUnitsAndFragmentOutputs   = 8;
            built_in_res.maxCombinedShaderOutputResources          = 8;
            built_in_res.maxImageSamples                           = 0;
            built_in_res.maxVertexImageUniforms                    = 0;
            built_in_res.maxTessControlImageUniforms               = 0;
            built_in_res.maxTessEvaluationImageUniforms            = 0;
            built_in_res.maxGeometryImageUniforms                  = 0;
            built_in_res.maxFragmentImageUniforms                  = 8;
            built_in_res.maxCombinedImageUniforms                  = 8;
            built_in_res.maxGeometryTextureImageUnits              = 16;
            built_in_res.maxGeometryOutputVertices                 = 256;
            built_in_res.maxGeometryTotalOutputComponents          = 1024;
            built_in_res.maxGeometryUniformComponents              = 1024;
            built_in_res.maxGeometryVaryingComponents              = 64;
            built_in_res.maxTessControlInputComponents             = 128;
            built_in_res.maxTessControlOutputComponents            = 128;
            built_in_res.maxTessControlTextureImageUnits           = 16;
            built_in_res.maxTessControlUniformComponents           = 1024;
            built_in_res.maxTessControlTotalOutputComponents       = 4096;
            built_in_res.maxTessEvaluationInputComponents          = 128;
            built_in_res.maxTessEvaluationOutputComponents         = 128;
            built_in_res.maxTessEvaluationTextureImageUnits        = 16;
            built_in_res.maxTessEvaluationUniformComponents        = 1024;
            built_in_res.maxTessPatchComponents                    = 120;
            built_in_res.maxPatchVertices                          = 32;
            built_in_res.maxTessGenLevel                           = 64;
            built_in_res.maxViewports                              = 16;
            built_in_res.maxVertexAtomicCounters                   = 0;
            built_in_res.maxTessControlAtomicCounters              = 0;
            built_in_res.maxTessEvaluationAtomicCounters           = 0;
            built_in_res.maxGeometryAtomicCounters                 = 0;
            built_in_res.maxFragmentAtomicCounters                 = 8;
            built_in_res.maxCombinedAtomicCounters                 = 8;
            built_in_res.maxAtomicCounterBindings                  = 1;
            built_in_res.maxVertexAtomicCounterBuffers             = 0;
            built_in_res.maxTessControlAtomicCounterBuffers        = 0;
            built_in_res.maxTessEvaluationAtomicCounterBuffers     = 0;
            built_in_res.maxGeometryAtomicCounterBuffers           = 0;
            built_in_res.maxFragmentAtomicCounterBuffers           = 1;
            built_in_res.maxCombinedAtomicCounterBuffers           = 1;
            built_in_res.maxAtomicCounterBufferSize                = 16384;
            built_in_res.maxTransformFeedbackBuffers               = 4;
            built_in_res.maxTransformFeedbackInterleavedComponents = 64;
            built_in_res.maxCullDistances                          = 8;
            built_in_res.maxCombinedClipAndCullDistances           = 8;
            built_in_res.maxSamples                                = 4;
            built_in_res.maxMeshOutputVerticesNV                   = 256;
            built_in_res.maxMeshOutputPrimitivesNV                 = 512;
            built_in_res.maxMeshWorkGroupSizeX_NV                  = 32;
            built_in_res.maxMeshWorkGroupSizeY_NV                  = 1;
            built_in_res.maxMeshWorkGroupSizeZ_NV                  = 1;
            built_in_res.maxTaskWorkGroupSizeX_NV                  = 32;
            built_in_res.maxTaskWorkGroupSizeY_NV                  = 1;
            built_in_res.maxTaskWorkGroupSizeZ_NV                  = 1;
            built_in_res.maxMeshViewCountNV                        = 4;
            built_in_res.maxDualSourceDrawBuffersEXT               = 1;

            built_in_res.limits.nonInductiveForLoops                 = true;
            built_in_res.limits.whileLoops                           = true;
            built_in_res.limits.doWhileLoops                         = true;
            built_in_res.limits.generalUniformIndexing               = true;
            built_in_res.limits.generalAttributeMatrixVectorIndexing = true;
            built_in_res.limits.generalVaryingIndexing               = true;
            built_in_res.limits.generalSamplerIndexing               = true;
            built_in_res.limits.generalVariableIndexing              = true;
            built_in_res.limits.generalConstantMatrixVectorIndexing  = true;

            const int default_version = 100;

            if (!shader.parse(&built_in_res, default_version, true, messages)) {
                WG_LOG_ERROR("failed to parse shader Num=" << Enum::to_str(language) << ": " << shader.getInfoLog());
                WG_LOG_INFO(source);
                m_message = shader.getInfoLog();
                m_status.store(GfxShaderStatus::Failed);
                return false;
            }

            return true;
        };

        glslang::TProgram              program;
        fast_vector<glslang::TShader*> shaders;

        glslang::TShader shader_vertex(EShLangVertex);
        glslang::TShader shader_fragment(EShLangFragment);
        glslang::TShader shader_compute(EShLangCompute);

        if (compile_vert_frag) {
            shaders.push_back(&shader_vertex);
            shaders.push_back(&shader_fragment);

            if (!compile_single_shader(EShLangVertex, *shaders[0], m_sources[0])) {
                return;
            }
            if (!compile_single_shader(EShLangFragment, *shaders[1], m_sources[1])) {
                return;
            }

            program.addShader(shaders[0]);
            program.addShader(shaders[1]);
        }

        if (compile_compute) {
            shaders.push_back(&shader_compute);

            if (!compile_single_shader(EShLangCompute, *shaders.back(), m_sources[0])) {
                return;
            }

            program.addShader(shaders[0]);
        }

        if (!program.link(messages)) {
            WG_LOG_ERROR("failed to link program: " << program.getInfoLog());
            m_message = program.getInfoLog();
            m_status.store(GfxShaderStatus::Failed);
            return;
        }

        fast_vector<std::vector<uint32_t>> spirvs;
        fast_vector<Ref<Data>>             spirv_datas;

        auto extract_spirv = [&](EShLanguage langugage, std::vector<uint32_t>& spirv, Ref<Data>& spirv_data) {
            glslang::TIntermediate* intermediate = program.getIntermediate(langugage);
            spv::SpvBuildLogger     logger;
            glslang::SpvOptions     spv_options;
            spv_options.disableOptimizer = false;
            spv_options.validate         = true;
            spv_options.optimizeSize     = true;
            glslang::GlslangToSpv(*intermediate, spirv, &logger, &spv_options);

            if (spirv.empty()) {
                WG_LOG_ERROR("failed to compile program: " << logger.getAllMessages());
                m_message = logger.getAllMessages();
                m_status.store(GfxShaderStatus::Failed);
                return false;
            }

            spirv_data = make_ref<Data>(spirv.data(), sizeof(uint32_t) * spirv.size());
            return true;
        };

        for (auto& shader : shaders) {
            auto& spirv      = spirvs.emplace_back();
            auto& spirv_data = spirv_datas.emplace_back();

            if (!extract_spirv(shader->getStage(), spirv, spirv_data)) {
                return;
            }
        }

        if (!program.buildReflection()) {
            WG_LOG_ERROR("failed to build program reflection: " << name());
            m_message = "failed to build reflection";
            m_status.store(GfxShaderStatus::Failed);
            return;
        }

        reflect(program);
        gen_byte_code(spirv_datas);

        timer.stop();
        WG_LOG_INFO("compiled (source): " << name() << ", code " << m_byte_code->size_as_kib() << " KiB, time: " << timer.get_elapsed_sec() << " sec");

        init(spirv_datas);
    }
    void VKShader::compile_from_byte_code() {
        WG_AUTO_PROFILE_VULKAN("VKShader::compile_from_byte_code");

        Timer timer;
        timer.start();

        ArchiveReaderMemory archive(m_byte_code->buffer(), m_byte_code->size());

        VKShaderBinary binary;
        if (!archive_read(archive, binary)) {
            WG_LOG_ERROR("failed to read shader binary " << name());
            return;
        }

        for (const auto& layout : binary.layouts) {
            m_set_layouts.push_back(m_driver.make_desc_layout(layout, Strid()));
        }

        m_reflection = std::move(binary.reflection);

        timer.stop();
        WG_LOG_INFO("compiled (byte code): " << name() << " time: " << timer.get_elapsed_sec() << " sec");

        init(binary.spirvs);
    }
    void VKShader::reflect(glslang::TProgram& program) {
        WG_AUTO_PROFILE_VULKAN("VKShader::reflect");

        m_reflection.ub_buffers_per_desc.fill(0);
        m_reflection.sb_buffers_per_desc.fill(0);
        m_reflection.textures_per_desc.fill(0);

        auto blocks_count = program.getNumUniformBlocks();
        for (int i = 0; i < blocks_count; ++i) {
            auto& block     = program.getUniformBlock(i);
            auto& qualifier = block.getType()->getQualifier();

            GfxShaderReflection::Buffer buffer;
            buffer.name    = SID(block.name);
            buffer.set     = qualifier.layoutSet;
            buffer.binding = qualifier.layoutBinding;
            buffer.size    = block.size;

            // Buffers treated in the same way, so check qualifier
            // see for more info https://github.com/KhronosGroup/SPIRV-Reflect/issues/67
            bool is_uniform = qualifier.storage == glslang::EvqUniform;
            bool is_storage = qualifier.storage == glslang::EvqBuffer;

            assert(is_uniform || is_storage);

            if (is_uniform) {
                m_reflection.ub_buffers[buffer.name] = buffer;
                m_reflection.ub_buffers_per_desc[buffer.set] += 1;
            }
            if (is_storage) {
                m_reflection.sb_buffers[buffer.name] = buffer;
                m_reflection.sb_buffers_per_desc[buffer.set] += 1;
            }
        }

        auto uniforms_count = program.getNumUniformVariables();
        for (int i = 0; i < uniforms_count; ++i) {
            auto& uniform = program.getUniform(i);

            if (uniform.getBinding() >= 0 && uniform.getType()->getBasicType() == glslang::EbtSampler) {
                auto get_sample_type = [](glslang::TSamplerDim dim, int arrayed, GfxTex& type) {
                    switch (dim) {
                        case glslang::Esd2D:
                            type = arrayed ? GfxTex::Tex2dArray : GfxTex::Tex2d;
                            break;
                        case glslang::EsdCube:
                            type = GfxTex::TexCube;
                            break;
                        default:
                            WG_LOG_ERROR("unsupported TSamplerDim");
                            break;
                    }
                };

                auto  type      = uniform.getType();
                auto& qualifier = type->getQualifier();

                GfxShaderReflection::Texture texture;
                texture.name       = SID(uniform.name);
                texture.set        = qualifier.layoutSet;
                texture.binding    = qualifier.layoutBinding;
                texture.array_size = uniform.size;

                get_sample_type(type->getSampler().dim, type->getSampler().arrayed, texture.tex);

                m_reflection.textures[texture.name] = texture;
                m_reflection.textures_per_desc[texture.set] += texture.array_size;
            }
        }
    }
    void VKShader::gen_byte_code(const fast_vector<Ref<Data>>& spirvs) {
        WG_AUTO_PROFILE_VULKAN("VKShader::gen_byte_code");

        VKShaderBinary binary;
        binary.spirvs     = spirvs;
        binary.reflection = m_reflection;

        for (auto& layout : m_set_layouts) {
            binary.layouts.push_back(layout->desc());
        }

        ArchiveWriterMemory archive;

        if (!archive_write(archive, binary)) {
            WG_LOG_ERROR("failed to create shader binary " << name());
            return;
        }

        m_byte_code = make_ref<Data>(archive.get_data().data(), archive.get_data().size());
    }
    void VKShader::init(const fast_vector<Ref<Data>>& spirvs) {
        WG_AUTO_PROFILE_VULKAN("VKShader::init");

        for (auto& binary : spirvs) {
            VkShaderModuleCreateInfo create_info{};
            create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            create_info.codeSize = static_cast<uint32_t>(binary->size());
            create_info.pCode    = reinterpret_cast<std::uint32_t*>(binary->buffer());

            VkShaderModule module;
            WG_VK_CHECK(vkCreateShaderModule(m_driver.device(), &create_info, nullptr, &module));
            WG_VK_NAME(m_driver.device(), module, VK_OBJECT_TYPE_SHADER_MODULE, "shader " + name().str());
            m_modules.push_back(module);
        }

        VkDescriptorSetLayout layouts[GfxLimits::MAX_DESC_SETS];
        std::uint32_t         layouts_count = 0;

        for (const auto& layout : m_set_layouts) {
            assert(layouts_count < GfxLimits::MAX_DESC_SETS);
            layouts[layouts_count++] = layout.cast<VKDescSetLayout>()->layout();
        }

        VkPipelineLayoutCreateInfo layout_create_info{};
        layout_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layout_create_info.setLayoutCount         = layouts_count;
        layout_create_info.pSetLayouts            = layouts;
        layout_create_info.pushConstantRangeCount = 0;
        layout_create_info.pPushConstantRanges    = nullptr;
        WG_VK_CHECK(vkCreatePipelineLayout(m_driver.device(), &layout_create_info, nullptr, &m_layout));
        WG_VK_NAME(m_driver.device(), m_layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "pso_layout " + name().str());

        m_status.store(GfxShaderStatus::Compiled);
    }

}// namespace wmoge