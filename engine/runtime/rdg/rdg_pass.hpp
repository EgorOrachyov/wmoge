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

#include "core/buffered_vector.hpp"
#include "core/flat_set.hpp"
#include "core/mask.hpp"
#include "core/simple_id.hpp"
#include "core/status.hpp"
#include "rdg/rdg_resources.hpp"

#include <functional>

namespace wmoge {

    /** @brief Rdg pass usage flag */
    enum class RDGPassFlag {
        ComputePass,
        GraphicsPass,
        MaterialPass,
        CopyPass
    };

    /** @brief Rdg pass flags */
    using RDGPassFlags = Mask<RDGPassFlag>;

    /** @brief Rdg pass id within graph */
    using RDGPassId = SimpleId<std::uint32_t>;

    /** @brief Rdg pass callback called on pass execution */
    using RDGPassCallback = std::function<Status()>;

    /** @brief Rdg pass color target info */
    struct RDGPassColorTarget {
        RDGTexture* resource = nullptr;
        Color4f     color    = Color4f();
        bool        clear    = false;
        GfxRtOp     op       = GfxRtOp::LoadStore;
    };

    /** @brief Rdg pass depth stencil target info */
    struct RDGPassDepthTarget {
        RDGTexture* resource      = nullptr;
        float       depth         = 1.0f;
        int         stencil       = 0;
        bool        clear_depth   = false;
        bool        clear_stencil = false;
        GfxRtOp     op            = GfxRtOp::LoadStore;
    };

    /** @brief Rdg pass referenced resource for manual usage */
    struct RDGPassResource {
        RDGResource* resource = nullptr;
        GfxAccess    access   = GfxAccess::None;
    };

    /**
     * @class RDGPass
     * @brief Represents single pass in a rgd graph for execution
    */
    class RDGPass {
    public:
        RDGPass(class RDGGraph& graph, Strid name, RDGPassId id, RDGPassFlags flags);

        RDGPass& color_target(RDGTexture* target);
        RDGPass& color_target(RDGTexture* target, const Color4f& clear_color);
        RDGPass& depth_target(RDGTexture* target);
        RDGPass& depth_target(RDGTexture* target, float clear_depth, int clear_stencil);
        RDGPass& reference(RDGResource* resource, GfxAccess access);
        RDGPass& reading(RDGBuffer* resource);
        RDGPass& writing(RDGBuffer* resource);
        RDGPass& bind(RDGPassCallback callback);

        [[nodiscard]] bool                has_resource(RDGResource* r) const;
        [[nodiscard]] const RDGPassFlags& get_flags() const { return m_flags; }
        [[nodiscard]] const RDGPassId&    get_id() const { return m_id; }
        [[nodiscard]] const Strid&        get_name() const { return m_name; }

    private:
        friend RDGGraph;

        buffered_vector<RDGPassColorTarget, 6> m_color_targets;
        buffered_vector<RDGPassDepthTarget, 1> m_depth_targets;
        buffered_vector<RDGPassResource, 16>   m_resources;
        flat_set<RDGResource*>                 m_referenced;

        RDGPassCallback m_callback;
        RDGPassFlags    m_flags;
        RDGPassId       m_id;
        Strid           m_name;

        class RDGGraph& m_graph;
    };

}// namespace wmoge