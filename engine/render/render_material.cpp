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

#include "render_material.hpp"

#include "core/crc32.hpp"
#include "core/engine.hpp"
#include "gfx/gfx_driver.hpp"
#include "resource/material.hpp"

namespace wmoge {

    RenderMaterial::RenderMaterial(class Material* material, std::size_t num_textures, std::size_t buffer_size) {
        assert(material);

        auto* gfx_driver = Engine::instance()->gfx_driver();

        m_material = material;
        m_textures.resize(num_textures);
        m_samplers.resize(num_textures);
        m_parameters = gfx_driver->make_uniform_buffer(int(buffer_size), GfxMemUsage::GpuLocal, m_material->get_name());
        m_hash       = Crc32::hash(&m_material, sizeof(m_material));
    }

    void RenderMaterial::ensure_version() {
        assert(m_material);

        GfxDriver*    gfx_driver = Engine::instance()->gfx_driver();
        ref_ptr<Data> data;
        std::size_t   new_version = m_version.load();

        if (m_material->copy_state(new_version, m_textures.data(), m_samplers.data(), data)) {
            m_version.store(new_version);
            gfx_driver->update_uniform_buffer(m_parameters, 0, int(data->size()), data);
        }
    }

    bool RenderMaterial::is_actual_version(std::size_t ref_version) {
        return ref_version == m_version.load();
    }

}// namespace wmoge