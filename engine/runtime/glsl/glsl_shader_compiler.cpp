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

#include "glsl_shader_compiler.hpp"

#include "core/string_utils.hpp"
#include "core/task.hpp"
#include "core/timer.hpp"
#include "glsl/glsl_builder.hpp"
#include "glsl/glsl_include_processor.hpp"
#include "grc/shader_compiler_task_manager.hpp"
#include "io/archive.hpp"
#include "io/enum.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

#include <sstream>

namespace wmoge {

    GlslShaderCompiler::GlslShaderCompiler() {
        if (!glslang::InitializeProcess()) {
            WG_LOG_ERROR("failed to init glslang");
        }

        m_file_system = IocContainer::iresolve_v<FileSystem>();
    }

    GlslShaderCompiler::~GlslShaderCompiler() {
        glslang::FinalizeProcess();
    }

    Status GlslShaderCompiler::compile(ShaderCompilerRequest& request) {
        WG_AUTO_PROFILE_VULKAN("GlslShaderCompiler::compile");

        Timer timer;
        timer.start();

        const ShaderCompilerInput& input  = request.input;
        ShaderCompilerOutput&      output = request.output;

        output.status = StatusCode::FailedCompile;

        if (input.language != GfxShaderLang::GlslVk450) {
            WG_LOG_ERROR("unsupported shader lang requested for " << input.name);
            return StatusCode::InvalidParameter;
        }

        auto       fs           = m_file_system;
        const int  glsl_version = 450;
        const bool core_profile = true;
        const bool collect_code = true;
        const auto messages     = static_cast<EShMessages>(EShMsgDefault | EShMsgCascadingErrors);

        auto dump_file = [&](const std::string& code) {
            WG_AUTO_PROFILE_VULKAN("GlslShaderCompiler::dump_file");

            if (input.options.dump_on_failure) {
                std::stringstream file_content;
                std::string       file_name = "debug/" + StringUtils::to_sha256(code).to_string() + ".txt";

                file_content << code << "\n";
                file_content << "\n\n";
                file_content << "=== Compilation info ===\n";
                file_content << input.name;

                fs->save_file(file_name, file_content.str());
                WG_LOG_ERROR("failed request " << input.name << " written to " << file_name);
            }
        };

        auto compile_single_shader = [&](EShLanguage language, glslang::TShader& shader, const ShaderCompilerInputFile& file, Sha256& source_hash) {
            WG_AUTO_PROFILE_VULKAN("GlslShaderCompiler::compile_shader");

            GlslIncludeProcessor include_processor(input.env, *fs, collect_code);

            if (!include_processor.parse_file(file.file_path)) {
                WG_LOG_ERROR("failed to process includes for " << input.name << " module=" << Enum::to_str(file.module_type));
                return StatusCode::Error;
            }

            GlslBuilder builder;
            builder.set_version(glsl_version, core_profile);
            builder.set_module(file.module_type);

            for (const auto& def : input.env.defines) {
                if (!def.second.empty()) {
                    builder.add_define(def.first, def.second);
                } else {
                    builder.add_define(def.first);
                }
            }

            builder.add_source(include_processor.get_result());

            const std::string source_code = builder.emit();
            source_hash                   = StringUtils::to_sha256(source_code);

            const char* shader_sources[1] = {source_code.data()};
            const int   shader_lengths[1] = {int(source_code.size())};

            shader.setStringsWithLengths(shader_sources, shader_lengths, 1);

            int                               version        = 100;
            glslang::EShClient                client         = glslang::EShClientVulkan;
            glslang::EShTargetClientVersion   client_version = glslang::EShTargetVulkan_1_0;
            glslang::EShTargetLanguageVersion target_version = glslang::EShTargetSpv_1_0;

            shader.setEnvInput(glslang::EShSourceGlsl, language, client, version);
            shader.setEnvClient(glslang::EShClientVulkan, client_version);
            shader.setEnvTarget(glslang::EShTargetSpv, target_version);
            shader.setEntryPoint(file.entry_point.c_str());

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
                WG_LOG_ERROR("failed to parse shader " << input.name << " module=" << Enum::to_str(file.module_type) << " : " << shader.getInfoLog());
                output.errors.push_back(shader.getInfoLog());
                dump_file(source_code);
                return StatusCode::Error;
            }

            return WG_OK;
        };

        glslang::TProgram                  program;
        buffered_vector<glslang::TShader*> shaders;

        glslang::TShader shader_vertex(EShLangVertex);
        glslang::TShader shader_fragment(EShLangFragment);
        glslang::TShader shader_compute(EShLangCompute);

        Status ok = WG_OK;

        for (const ShaderCompilerInputFile& file : input.files) {
            EShLanguage language;

            switch (file.module_type) {
                case GfxShaderModule::Vertex:
                    language = EShLangVertex;
                    shaders.push_back(&shader_vertex);
                    break;
                case GfxShaderModule::Fragment:
                    language = EShLangFragment;
                    shaders.push_back(&shader_fragment);
                    break;
                case GfxShaderModule::Compute:
                    shaders.push_back(&shader_compute);
                    language = EShLangCompute;
                    break;
                default:
                    WG_LOG_ERROR("Unsupported module type");
                    return StatusCode::Error;
            }

            auto& source_hash = output.source_hashes.emplace_back();

            Status s = compile_single_shader(language, *shaders.back(), file, source_hash);

            if (!s) {
                ok = s;
                continue;
            }

            program.addShader(shaders.back());
        }

        if (!ok) {
            WG_LOG_ERROR("failed to compile some shader modules");
            return StatusCode::Error;
        }

        {
            WG_AUTO_PROFILE_VULKAN("GlslShaderCompiler::link_program");

            if (!program.link(messages)) {
                WG_LOG_ERROR("failed to link program: " << program.getInfoLog());
                output.errors.push_back(program.getInfoLog());
                return StatusCode::Error;
            }
        }

        buffered_vector<std::vector<uint32_t>> spirvs;
        std::size_t                            spirv_size = 0;

        auto extract_spirv = [&](EShLanguage langugage, std::vector<uint32_t>& spirv, Ref<Data>& spirv_data, Sha256& spirv_hash) {
            WG_AUTO_PROFILE_VULKAN("GlslShaderCompiler::extract_spirv");

            glslang::SpvOptions spv_options;

            ShaderCompilerOptions options = input.options;
            spv_options.disableOptimizer  = options.disable_otimizer;
            spv_options.generateDebugInfo = options.generate_degug_info;
            spv_options.stripDebugInfo    = options.strip_debug_info;
            spv_options.validate          = options.validate;
            spv_options.optimizeSize      = options.optimize_size;

            spv::SpvBuildLogger     logger;
            glslang::TIntermediate* intermediate = program.getIntermediate(langugage);
            glslang::GlslangToSpv(*intermediate, spirv, &logger, &spv_options);

            if (spirv.empty()) {
                WG_LOG_ERROR("failed to compile program: " << logger.getAllMessages());
                output.errors.push_back(logger.getAllMessages());
                return StatusCode::Error;
            }

            spirv_data = make_ref<Data>(spirv.data(), sizeof(uint32_t) * spirv.size());
            spirv_hash = spirv_data->to_sha256();
            spirv_size += spirv_data->size();

            return WG_OK;
        };

        for (auto& shader : shaders) {
            auto& spirv      = spirvs.emplace_back();
            auto& spirv_data = output.bytecode.emplace_back();
            auto& spirv_hash = output.bytecode_hashes.emplace_back();

            Status s = extract_spirv(shader->getStage(), spirv, spirv_data, spirv_hash);

            if (!s) {
                ok = s;
            }
        }

        timer.stop();
        WG_LOG_INFO("compiled: " << input.name << " size=" << StringUtils::from_mem_size(spirv_size) << ", time=" << timer.get_elapsed_sec() << "sec");

        request.output.time_sec = timer.get_elapsed_sec();
        request.output.status   = ok;

        return ok;
    }

    GlslShaderCompilerAdapter::GlslShaderCompilerAdapter(GfxShaderPlatform platform) {
        m_platform      = platform;
        m_glsl_compiler = IocContainer::iresolve_v<GlslShaderCompiler>();
        m_task_manager  = IocContainer::iresolve_v<ShaderCompilerTaskManager>();
    }

    Async GlslShaderCompilerAdapter::compile(const Ref<ShaderCompilerRequest>& request, const Async& depends_on) {
        Task task(request->input.name, [compiler = m_glsl_compiler, request](TaskContext&) {
            if (!compiler->compile(*request)) {
                return 1;
            }
            return 0;
        });
        task.set_task_manager(*m_task_manager);
        return task.schedule(depends_on).as_async();
    }

    std::shared_ptr<ShaderCodeBuilder> GlslShaderCompilerAdapter::make_builder() {
        return std::make_shared<GlslBuilder>();
    }

    GfxShaderPlatform GlslShaderCompilerAdapter::get_platform() {
        return m_platform;
    }

    GfxShaderLang GlslShaderCompilerAdapter::get_lang() {
        return GfxShaderLang::GlslVk450;
    }

}// namespace wmoge