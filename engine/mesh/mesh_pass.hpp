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

#pragma once

#include "core/array_view.hpp"
#include "core/fast_vector.hpp"
#include "core/mask.hpp"
#include "core/status.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_vert_format.hpp"

#include <string>

namespace wmoge {

    /** @brief Supported engine mesh pass types */
    enum class MeshPassType {
        Background = 0,//<
        Shadow,        //< Pass: Shadow cache generation for lights
        GBuffer,       //< Pass: GBuffer generation for opaque geometry
        Forward,       //< Pass: Forward for transparent geometry and complex shading
        Ui,            //<
        Outline,       //<
        Overlay,       //<
        Total = 7
    };

    /** @brief Total num of mesh passes */
    static constexpr int MESH_PASSES_TOTAL = static_cast<int>(MeshPassType::Total);

    /**
     * @class MeshPass
     * @brief Compiled state required to draw a mesh subset into a particular pass
    */
    struct MeshPass {
        GfxVertBuffersSetup vert_setup;
        GfxIndexBufferSetup index_setup;
        Ref<GfxVertFormat>  format;
        Ref<GfxPipeline>    pipeline;
        MeshPassType        pass_type = MeshPassType::Total;
    };

    /**
     * @class MeshPassList
     * @brief List with compiled PSO states to render mesh in multiple passes
    */
    class MeshPassList {
    public:
        static constexpr int NUM_PASSES_INLINE = 3;

        bool                     has_pass(MeshPassType pass_type) const;
        std::optional<MeshPass*> get_pass(MeshPassType pass_type);
        void                     add_pass(MeshPass&& pass, bool overwrite = true);

        [[nodiscard]] const Mask<MeshPassType>& get_mask() const { return m_mask; }
        [[nodiscard]] std::size_t               get_size() const { return m_passes.size(); }
        [[nodiscard]] bool                      is_empty() const { return m_passes.empty(); }

    private:
        fast_vector<MeshPass, NUM_PASSES_INLINE> m_passes;
        Mask<MeshPassType>                       m_mask;
    };

    /**
     * @class MeshPassProcessor
     * @brief Responsible for compiling a mesh batch into a draw call for a particular pass
     */
    class MeshPassProcessor {
    public:
        MeshPassProcessor();
        virtual ~MeshPassProcessor() = default;

        virtual bool         filter(const struct MeshBatch& batch)                                  = 0;
        virtual Status       compile(const struct MeshBatch& batch, Ref<GfxPipeline>& out_pipeline) = 0;
        virtual std::string  get_name() const                                                       = 0;
        virtual MeshPassType get_pass_type() const                                                  = 0;

    protected:
        class ShaderManager* m_shader_manager = nullptr;
        class GfxDriver*     m_gfx_driver     = nullptr;
    };

}// namespace wmoge