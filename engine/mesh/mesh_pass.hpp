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

#ifndef WMOGE_MESH_PASS_HPP
#define WMOGE_MESH_PASS_HPP

#include "core/array_view.hpp"
#include "core/fast_vector.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_pipeline.hpp"

namespace wmoge {

    /** @brief Supported engine mesh pass types */
    enum class MeshPassType {
        None = 0,  //< Default invalid
        Background,//<
        Shadow,    //< Shadow cache generation for lights
        GBuffer,   //< GBuffer generation for opaque geometry
        Forward,   //<
        Trasparent,//< Objects which require transparancy
        Pfx,       //< Particles simulation
        Ui,        //<
        Outline,   //<
        Overlay,   //<
        Total = 8
    };

    /**
     * @class MeshPassList
     * @brief List with compiled PSO states to render mesh in multiple passes
     */
    class MeshPassList {
    public:
        static constexpr int NUM_PASSES_TOTAL  = static_cast<int>(MeshPassType::Total);
        static constexpr int NUM_PASSES_INLINE = 3;

        bool                            has_pass(MeshPassType pass_type) const;
        std::optional<Ref<GfxPipeline>> get_pass(MeshPassType pass_type) const;
        void                            add_pass(Ref<GfxPipeline> pass, MeshPassType pass_type, bool overwrite = true);

        [[nodiscard]] ArrayView<const Ref<GfxPipeline>> get_pipelines() const { return m_pipelines; }
        [[nodiscard]] ArrayView<const MeshPassType>     get_types() const { return m_types; }
        [[nodiscard]] const Mask<MeshPassType>&         get_mask() const { return m_mask; }
        [[nodiscard]] std::size_t                       get_size() const { return m_pipelines.size(); }
        [[nodiscard]] bool                              is_empty() const { return m_pipelines.empty(); }

    private:
        fast_vector<Ref<GfxPipeline>, NUM_PASSES_INLINE> m_pipelines;
        fast_vector<MeshPassType, NUM_PASSES_INLINE>     m_types;
        Mask<MeshPassType>                               m_mask;
    };

}// namespace wmoge

#endif//WMOGE_MESH_PASS_HPP