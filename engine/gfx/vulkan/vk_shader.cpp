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
#include "vk_driver.hpp"

#include "core/timer.hpp"
#include "debug/profiler.hpp"
#include "io/archive.hpp"
#include "io/archive_memory.hpp"

namespace wmoge {

    VKShader::VKShader(class VKDriver& driver) : VKResource<GfxShader>(driver) {
    }
    VKShader::~VKShader() {
        WG_AUTO_PROFILE_VULKAN("VKShader::~VKShader");

        for (auto& module : m_modules) {
            vkDestroyShaderModule(m_driver.device(), module, nullptr);
        }
        if (m_layout) {
            vkDestroyPipelineLayout(m_driver.device(), m_layout, nullptr);
        }
        for (auto set_layout : m_set_layouts) {
            if (set_layout) {
                vkDestroyDescriptorSetLayout(m_driver.device(), set_layout, nullptr);
            }
        }
    }
    void VKShader::setup(std::string vertex, std::string fragment, const StringId& name) {
        WG_AUTO_PROFILE_VULKAN("VKShader::configure");

        m_name = name;
        m_sources.push_back(std::move(vertex));
        m_sources.push_back(std::move(fragment));
    }
    void VKShader::setup(Ref<Data> byte_code, const wmoge::StringId& name) {
        WG_AUTO_PROFILE_VULKAN("VKShader::configure");

        m_name      = name;
        m_byte_code = std::move(byte_code);
    }
    GfxShaderStatus VKShader::status() const {
        return m_status.load();
    }
    std::string VKShader::message() const {
        return status() != GfxShaderStatus::Compiling ? m_message : std::string();
    }
    const GfxShaderReflection* VKShader::reflection() const {
        return status() == GfxShaderStatus::Compiled ? &m_reflection : nullptr;
    }
    Ref<Data> VKShader::byte_code() const {
        return status() == GfxShaderStatus::Compiled ? m_byte_code : Ref<Data>();
    }
    void VKShader::compile_from_source() {
        WG_AUTO_PROFILE_VULKAN("VKShader::compile_from_source");

        Timer timer;
        timer.start();

        glslang::TProgram program;
        glslang::TShader  shaders[2] = {glslang::TShader(EShLangVertex), glslang::TShader(EShLangFragment)};
        EShLanguage       types[2]   = {EShLangVertex, EShLangFragment};

        const char* sources[2] = {m_sources[0].c_str(), m_sources[1].c_str()};
        const int   lengths[2] = {static_cast<int>(m_sources[0].size()), static_cast<int>(m_sources[1].size())};

        EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

        for (int i = 0; i < 2; i++) {
            auto  language = types[i];
            auto& shader   = shaders[i];

            const char* shader_sources[1] = {sources[i]};
            const int   shader_lengths[1] = {lengths[i]};

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
                WG_LOG_ERROR("failed to parse shader: " << shader.getInfoLog());
                m_message = shader.getInfoLog();
                m_status.store(GfxShaderStatus::Failed);
                return;
            }

            program.addShader(&shader);
        }

        if (!program.link(messages)) {
            WG_LOG_ERROR("failed to link program: " << program.getInfoLog());
            m_message = program.getInfoLog();
            m_status.store(GfxShaderStatus::Failed);
            return;
        }

        std::vector<uint32_t> spirv[2];

        for (int i = 0; i < 2; i++) {
            glslang::TIntermediate* intermediate = program.getIntermediate(types[i]);
            spv::SpvBuildLogger     logger;
            glslang::SpvOptions     spv_options;
            spv_options.disableOptimizer = false;
            spv_options.validate         = true;
            spv_options.optimizeSize     = true;
            glslang::GlslangToSpv(*intermediate, spirv[i], &logger, &spv_options);

            if (spirv[i].empty()) {
                WG_LOG_ERROR("failed to compile program: " << logger.getAllMessages());
                m_message = logger.getAllMessages();
                m_status.store(GfxShaderStatus::Failed);
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
        gen_byte_code(spirv[0], spirv[1]);

        timer.stop();
        WG_LOG_INFO("compiled (source): " << name() << ", code " << m_byte_code->size_as_kib() << " KiB, time: " << timer.get_elapsed_sec() << " sec");
        Ref<VKShader> this_shader(this);

        init(spirv[0], spirv[1]);
    }
    void VKShader::compile_from_byte_code() {
        WG_AUTO_PROFILE_VULKAN("VKShader::compile_from_byte_code");

        Timer timer;
        timer.start();

        ArchiveReaderMemory archive(m_byte_code->buffer(), m_byte_code->size());

        bool has_shader_vertex;
        bool has_shader_fragment;
        int  size_vertex;
        int  size_fragment;

        archive >> has_shader_vertex;
        archive >> has_shader_fragment;
        archive >> size_vertex;
        archive >> size_fragment;

        std::vector<uint32_t> vertex(size_vertex);
        std::vector<uint32_t> fragment(size_fragment);

        archive.nread(size_vertex * sizeof(uint32_t), vertex.data());
        archive.nread(size_fragment * sizeof(uint32_t), fragment.data());

        archive >> m_reflection;

        timer.stop();
        WG_LOG_INFO("compiled (byte code): " << name() << " time: " << timer.get_elapsed_sec() << " sec");

        init(vertex, fragment);
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
    void VKShader::gen_byte_code(const std::vector<uint32_t>& vertex, const std::vector<uint32_t>& fragment) {
        WG_AUTO_PROFILE_VULKAN("VKShader::gen_byte_code");

        bool has_shader_vertex   = true;
        bool has_shader_fragment = true;
        int  size_vertex         = static_cast<int>(vertex.size());
        int  size_fragment       = static_cast<int>(fragment.size());

        ArchiveWriterMemory archive;
        archive << has_shader_vertex;
        archive << has_shader_fragment;
        archive << size_vertex;
        archive << size_fragment;
        archive.nwrite(size_vertex * sizeof(uint32_t), vertex.data());
        archive.nwrite(size_fragment * sizeof(uint32_t), fragment.data());
        archive << m_reflection;

        m_byte_code = make_ref<Data>(archive.get_data().data(), archive.get_data().size());
    }
    void VKShader::init(const std::vector<uint32_t>& vertex, const std::vector<uint32_t>& fragment) {
        WG_AUTO_PROFILE_VULKAN("VKShader::init");

        const std::vector<uint32_t>* binaries[2] = {&vertex, &fragment};

        for (auto binary : binaries) {
            VkShaderModuleCreateInfo create_info{};
            create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            create_info.codeSize = static_cast<uint32_t>(binary->size() * sizeof(uint32_t));
            create_info.pCode    = binary->data();

            VkShaderModule module;
            WG_VK_CHECK(vkCreateShaderModule(m_driver.device(), &create_info, nullptr, &module));
            WG_VK_NAME(m_driver.device(), module, VK_OBJECT_TYPE_SHADER_MODULE, "shader@" + name().str());
            m_modules.push_back(module);
        }

        std::array<std::vector<VkDescriptorSetLayoutBinding>, GfxLimits::MAX_DESC_SETS> bindings{};

        for (auto& texture : m_reflection.textures) {
            auto& info                 = texture.second;
            auto& binding              = bindings[info.set].emplace_back();
            binding.binding            = info.binding;
            binding.descriptorCount    = info.array_size;
            binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            binding.stageFlags         = VK_SHADER_STAGE_ALL;
            binding.pImmutableSamplers = nullptr;
        }
        for (auto& buffer : m_reflection.ub_buffers) {
            auto& info                 = buffer.second;
            auto& binding              = bindings[info.set].emplace_back();
            binding.binding            = info.binding;
            binding.descriptorCount    = 1;
            binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.stageFlags         = VK_SHADER_STAGE_ALL;
            binding.pImmutableSamplers = nullptr;
        }
        for (auto& buffer : m_reflection.sb_buffers) {
            auto& info                 = buffer.second;
            auto& binding              = bindings[info.set].emplace_back();
            binding.binding            = info.binding;
            binding.descriptorCount    = 1;
            binding.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            binding.stageFlags         = VK_SHADER_STAGE_ALL;
            binding.pImmutableSamplers = nullptr;
        }

        for (int i = 0; i < GfxLimits::MAX_DESC_SETS; i++) {
            VkDescriptorSetLayoutCreateInfo desc_set_layout_info{};
            desc_set_layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            desc_set_layout_info.bindingCount = static_cast<uint32_t>(bindings[i].size());
            desc_set_layout_info.pBindings    = bindings[i].data();
            WG_VK_CHECK(vkCreateDescriptorSetLayout(m_driver.device(), &desc_set_layout_info, nullptr, &m_set_layouts[i]));
            WG_VK_NAME(m_driver.device(), m_set_layouts[i], VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, "layout_" + std::to_string(i) + "@" + name().str());
        }

        VkPipelineLayoutCreateInfo layout_create_info{};
        layout_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layout_create_info.setLayoutCount         = static_cast<uint32_t>(m_set_layouts.size());
        layout_create_info.pSetLayouts            = m_set_layouts.data();
        layout_create_info.pushConstantRangeCount = 0;
        layout_create_info.pPushConstantRanges    = nullptr;
        WG_VK_CHECK(vkCreatePipelineLayout(m_driver.device(), &layout_create_info, nullptr, &m_layout));
        WG_VK_NAME(m_driver.device(), m_layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "layout@" + name().str());

        m_status.store(GfxShaderStatus::Compiled);
    }

}// namespace wmoge