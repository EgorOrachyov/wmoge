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
#include "core/timer.hpp"
#include "io/archive.hpp"
#include "io/archive_memory.hpp"
#include "io/enum.hpp"
#include "profiler/profiler.hpp"

#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

namespace wmoge {

    GlslShaderCompiler::GlslShaderCompiler() {
        if (!glslang::InitializeProcess()) {
            WG_LOG_ERROR("failed to init glslang");
        }
    }

    GlslShaderCompiler::~GlslShaderCompiler() {
        glslang::FinalizeProcess();
    }

    Status GlslShaderCompiler::compile(const GlslCompilerInput& input, GlslCompilerOutput& output) {
        WG_AUTO_PROFILE_VULKAN("GlslShaderCompiler::compile");

        Timer timer;
        timer.start();

        const auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

        auto compile_single_shader = [&](EShLanguage language, glslang::TShader& shader, const GlslInputFile& file) {
            const char* shader_sources[1] = {file.source_code.data()};
            const int   shader_lengths[1] = {int(file.source_code.size())};

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
                WG_LOG_ERROR("failed to parse shader Num=" << Enum::to_str(language) << ": " << shader.getInfoLog());
                output.errors.push_back(shader.getInfoLog());
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

        for (const GlslInputFile& file : input.files) {
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

            Status s = compile_single_shader(language, *shaders.back(), file);

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

        if (!program.link(messages)) {
            WG_LOG_ERROR("failed to link program: " << program.getInfoLog());
            output.errors.push_back(program.getInfoLog());
            return StatusCode::Error;
        }

        buffered_vector<std::vector<uint32_t>> spirvs;
        std::size_t                            spirv_size = 0;

        auto extract_spirv = [&](EShLanguage langugage, std::vector<uint32_t>& spirv, Ref<Data>& spirv_data) {
            glslang::TIntermediate* intermediate = program.getIntermediate(langugage);
            spv::SpvBuildLogger     logger;
            glslang::SpvOptions     spv_options;
            spv_options.disableOptimizer = input.disable_otimizer;
            spv_options.validate         = input.validate;
            spv_options.optimizeSize     = input.optimize_size;
            glslang::GlslangToSpv(*intermediate, spirv, &logger, &spv_options);

            if (spirv.empty()) {
                WG_LOG_ERROR("failed to compile program: " << logger.getAllMessages());
                output.errors.push_back(logger.getAllMessages());
                return StatusCode::Error;
            }

            spirv_data = make_ref<Data>(spirv.data(), sizeof(uint32_t) * spirv.size());
            spirv_size += spirv_data->size();
            return WG_OK;
        };

        for (auto& shader : shaders) {
            auto& spirv      = spirvs.emplace_back();
            auto& spirv_data = output.bytecode.emplace_back();

            Status s = extract_spirv(shader->getStage(), spirv, spirv_data);

            if (!s) {
                ok = s;
            }
        }

        timer.stop();
        WG_LOG_INFO("compiled: " << input.name << " size=" << StringUtils::from_mem_size(spirv_size) << ", time=" << timer.get_elapsed_sec() << "sec");

        return ok;
    }

}// namespace wmoge