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
#include "gfx/gfx_driver.hpp"
#include "grc/shader_manager.hpp"
#include "rdg/rdg_pass.hpp"
#include "rdg/rdg_pool.hpp"
#include "rdg/rdg_resources.hpp"

#include <vector>

namespace wmoge {

    /** @brief Rdg mark for graph profiling */
    struct RdgProfileMark {
        std::string name;
        Strid       category;
        Strid       function;
        Strid       file;
        std::size_t line;
    };

    /** @brief Rdg scope for graph profiling */
    struct RdgProfileScope {
        RdgProfileScope(RdgProfileMark& mark, const std::string& data, class RdgGraph& graph);
        ~RdgProfileScope();

        class RdgGraph& graph;
    };

    /**
     * @class RdgGraph
     * @brief Complete graph of passes for execution on gpu
    */
    class RdgGraph {
    public:
        RdgGraph(RdgPool* pool, GfxDriver* driver, ShaderManager* shader_manager);

        RdgPass& add_pass(Strid name, RdgPassFlags flags = {});
        RdgPass& add_compute_pass(Strid name, RdgPassFlags flags = {});
        RdgPass& add_graphics_pass(Strid name, RdgPassFlags flags = {});
        RdgPass& add_material_pass(Strid name, RdgPassFlags flags = {});
        RdgPass& add_copy_pass(Strid name, RdgPassFlags flags = {});

        RdgTexture*       create_texture(const GfxTextureDesc& desc, Strid name);
        RdgTexture*       import_texture(const GfxTextureRef& texture);
        RdgTexture*       find_texture(const GfxTextureRef& texture);
        RdgStorageBuffer* create_storage_buffer(const GfxBufferDesc& desc, Strid name);
        RdgStorageBuffer* import_storage_buffer(const GfxStorageBufferRef& buffer);
        RdgStorageBuffer* find_storage_buffer(const GfxStorageBufferRef& buffer);
        RdgVertBuffer*    import_vert_buffer(const GfxVertBufferRef& buffer);
        RdgVertBuffer*    find_vert_buffer(const GfxVertBufferRef& buffer);

        Ref<Data>             make_upload_data(array_view<const std::uint8_t> buffer);
        Ref<ShaderParamBlock> make_param_block(Shader* shader, std::int16_t space_idx, const Strid& name);

        void push_event(RdgProfileMark* mark, const std::string& data);
        void pop_event();

        Status compile();
        Status execute();

        [[nodiscard]] GfxDriver*     get_driver() const { return m_driver; }
        [[nodiscard]] ShaderManager* get_shader_manager() const { return m_shader_manager; }

    private:
        void          add_resource(const RdgResourceRef& resource);
        RdgPassId     next_pass_id();
        RdgResourceId next_res_id();

    private:
        struct Event {
            RdgProfileMark* mark;
            std::string     data;
        };

        flat_map<GfxResource*, RdgResource*> m_resources_imported;
        pool_vector<RdgResourceRef>          m_resources;
        pool_vector<RdgPass>                 m_passes;
        RdgPassId                            m_next_pass_id{0};
        RdgResourceId                        m_next_res_id{0};
        RdgPool*                             m_pool           = nullptr;
        GfxDriver*                           m_driver         = nullptr;
        ShaderManager*                       m_shader_manager = nullptr;
        std::vector<Event>                   m_events;
    };

}// namespace wmoge

#define WG_PROFILE_RDG_MARK(var, system, name)                                                      \
    static RdgProfileMark var {                                                                     \
        std::string(name), SID(#system), SID(__FUNCTION__), SID(__FILE__), std::size_t { __LINE__ } \
    }

#define WG_PROFILE_RDG_SCOPE_WITH_DESC(graph, system, name, desc) \
    WG_PROFILE_RDG_MARK(__wg_auto_mark_rdg, system, name);        \
    RdgProfileScope __wg_auto_scope_gpu(__wg_auto_mark_rdg, desc, graph)

#define WG_PROFILE_RDG_SCOPE(graph, name) \
    WG_PROFILE_RDG_SCOPE_WITH_DESC(graph, gpurdg, name, "")