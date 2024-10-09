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

#include "core/flat_map.hpp"
#include "core/pool_vector.hpp"
#include "rdg/rdg_pass.hpp"
#include "rdg/rdg_pool.hpp"
#include "rdg/rdg_resources.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class RDGGraph
     * @brief Complete graph of passes for execution on gpu
    */
    class RDGGraph {
    public:
        RDGGraph(RDGPool* pool);

        RDGPass& add_pass(Strid name, RDGPassFlags flags = {});
        RDGPass& add_compute_pass(Strid name, RDGPassFlags flags = {});
        RDGPass& add_graphics_pass(Strid name, RDGPassFlags flags = {});
        RDGPass& add_material_pass(Strid name, RDGPassFlags flags = {});

        RDGTexture*       create_texture(const GfxTextureDesc& desc, Strid name);
        RDGTexture*       import_texture(const GfxTextureRef& texture);
        RDGTexture*       find_texture(const GfxTextureRef& texture);
        RDGStorageBuffer* create_storage_buffer(const GfxBufferDesc& desc, Strid name);
        RDGStorageBuffer* import_storage_buffer(const GfxStorageBufferRef& buffer);
        RDGStorageBuffer* find_storage_buffer(const GfxStorageBufferRef& buffer);

        Status compile();
        Status execute();

    private:
        void          add_resource(const RDGResourceRef& resource);
        RDGPassId     next_pass_id();
        RDGResourceId next_res_id();

    private:
        flat_map<GfxResource*, RDGResource*> m_resources_imported;
        pool_vector<RDGResourceRef>          m_resources;
        pool_vector<RDGPass>                 m_passes;
        RDGPassId                            m_next_pass_id{0};
        RDGResourceId                        m_next_res_id{0};
        RDGPool*                             m_pool = nullptr;
    };

}// namespace wmoge