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
#include "gfx/gfx_driver.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "rdg/rdg_pass.hpp"
#include "rdg/rdg_pool.hpp"
#include "rdg/rdg_resources.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace wmoge {

    /** @brief Rdg graph compilation options */
    struct RdgCompileOptions {
    };

    /** @brief Rdg graph execution options */
    struct RdgExecuteOptions {
    };

    /**
     * @class RdgGraph
     * @brief Complete graph of passes for execution on gpu
    */
    class RdgGraph {
    public:
        RdgGraph(RdgPool* pool, GfxDriver* driver, ShaderManager* shader_manager, TextureManager* texture_manager);

        void clear();

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
        RdgIndexBuffer*   import_index_buffer(const GfxIndexBufferRef& buffer);
        RdgIndexBuffer*   find_index_buffer(const GfxIndexBufferRef& buffer);
        RdgParamBlock*    create_param_block(const std::function<RdgParamBlockRef(RdgResourceId)>& factory);

        Ref<Data>             make_upload_data(array_view<const std::uint8_t> buffer);
        Ref<ShaderParamBlock> make_param_block(Shader* shader, std::int16_t space_idx, const Strid& name);

        void push_event(struct RdgProfileMark* mark, const std::string& data);
        void pop_event();

        Status compile(const RdgCompileOptions& options);
        Status execute(const RdgExecuteOptions& options);

        [[nodiscard]] GfxDriver*      get_driver() const { return m_driver; }
        [[nodiscard]] ShaderManager*  get_shader_manager() const { return m_shader_manager; }
        [[nodiscard]] Ref<GfxSampler> get_sampler(DefaultSampler sampler);

    private:
        Status        prepare_pass(RdgPassId pass_id, RdgPassContext& context);
        Status        execute_pass(RdgPassId pass_id, RdgPassContext& context);
        void          add_resource(const RdgResourceRef& resource, GfxAccess src_access, GfxAccess dst_access = GfxAccess::None);
        RdgPassId     next_pass_id();
        RdgResourceId next_res_id();

        using RdgEventId = int;

        struct RdgEvent {
            struct RdgProfileMark* mark;
            std::string            data;
        };

        struct RdgPassData {
            std::vector<RdgEventId> events_to_begin;
            int                     events_to_end = 0;
        };

        struct RdgResourceData {
            RdgResourceRef resource;
            GfxAccess      src_access = GfxAccess::None;
            GfxAccess      dst_access = GfxAccess::None;
        };

    private:
        flat_map<GfxResource*, RdgResource*> m_resources_imported;
        std::vector<RdgResourceData>         m_resources;
        std::vector<RdgPass>                 m_passes;
        std::vector<RdgPassData>             m_passes_data;
        std::vector<Ref<ShaderParamBlock>>   m_param_blocks;
        RdgPassId                            m_next_pass_id{0};
        RdgResourceId                        m_next_res_id{0};
        RdgPool*                             m_pool            = nullptr;
        GfxDriver*                           m_driver          = nullptr;
        ShaderManager*                       m_shader_manager  = nullptr;
        TextureManager*                      m_texture_manager = nullptr;
        std::vector<RdgEvent>                m_events;
        std::vector<RdgEventId>              m_events_stack;
        std::vector<GfxAccess>               m_resource_states;
    };

}// namespace wmoge
