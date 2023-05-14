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

#ifndef WMOGE_RENDER_MATERIAL_HPP
#define WMOGE_RENDER_MATERIAL_HPP

#include "core/crc32.hpp"
#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "resource/texture.hpp"

#include <atomic>
#include <cinttypes>

namespace wmoge {

    /**
     * @class RenderMaterialHash
     * @brief Hash of the material used for sorting
     */
    using RenderMaterialHash = Crc32Hash;

    /**
     * @class RenderMaterial
     * @brief Material proxy used for rendering in render engine
     */
    class RenderMaterial : public RefCnt {
    public:
        RenderMaterial(class Material* material, std::size_t num_textures, std::size_t buffer_size);

        void ensure_version();
        bool is_actual_version(std::size_t ref_version);

        fast_vector<Ref<GfxTexture>>& get_textures() { return m_textures; }
        fast_vector<Ref<GfxSampler>>& get_samplers() { return m_samplers; }
        Ref<GfxUniformBuffer>&        get_parameters() { return m_parameters; }

        const fast_vector<Ref<GfxTexture>>& get_textures() const { return m_textures; }
        const fast_vector<Ref<GfxSampler>>& get_samplers() const { return m_samplers; }
        const Ref<GfxUniformBuffer>&        get_parameters() const { return m_parameters; }

        std::size_t        get_version() const { return m_version.load(); }
        class Material*    get_material() const { return m_material; }
        RenderMaterialHash get_hash() const { return m_hash; }

    private:
        fast_vector<Ref<GfxTexture>> m_textures;
        fast_vector<Ref<GfxSampler>> m_samplers;
        Ref<GfxUniformBuffer>        m_parameters;
        std::atomic_size_t           m_version{0};
        class Material*              m_material = nullptr;
        RenderMaterialHash           m_hash{};
    };

}// namespace wmoge

#endif//WMOGE_RENDER_MATERIAL_HPP
