#include "rdg_pass.hpp"
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

#include "rdg_pass.hpp"

#include <cassert>

namespace wmoge {

    RDGPass::RDGPass(RDGGraph& graph, Strid name, RDGPassId id, RDGPassFlags flags) : m_graph(graph) {
        m_name  = name;
        m_id    = id;
        m_flags = flags;
    }

    RDGPass& RDGPass::color_target(RDGTexture* target) {
        RDGPassColorTarget& t = m_color_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::LoadStore;
        return reference(target, GfxAccess::RenderTarget);
    }

    RDGPass& RDGPass::color_target(RDGTexture* target, const Color4f& clear_color) {
        RDGPassColorTarget& t = m_color_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::ClearStore;
        t.color               = clear_color;
        t.clear               = true;
        return reference(target, GfxAccess::RenderTarget);
    }

    RDGPass& RDGPass::depth_target(RDGTexture* target) {
        assert(m_depth_targets.empty());
        RDGPassDepthTarget& t = m_depth_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::LoadStore;
        return reference(target, GfxAccess::RenderTarget);
    }

    RDGPass& RDGPass::depth_target(RDGTexture* target, float clear_depth, int clear_stencil) {
        RDGPassDepthTarget& t = m_depth_targets.emplace_back();
        t.resource            = target;
        t.op                  = GfxRtOp::ClearStore;
        t.depth               = clear_depth;
        t.stencil             = clear_stencil;
        t.clear_depth         = true;
        t.clear_stencil       = true;
        return reference(target, GfxAccess::RenderTarget);
    }

    RDGPass& RDGPass::reference(RDGResource* resource, GfxAccess access) {
        assert(resource);
        assert(!has_resource(resource));
        RDGPassResource& r = m_resources.emplace_back();
        r.resource         = resource;
        r.access           = access;
        m_referenced.insert(resource);
        return *this;
    }

    RDGPass& RDGPass::reading(RDGBuffer* resource) {
        return reference(resource, GfxAccess::BufferRead);
    }

    RDGPass& RDGPass::writing(RDGBuffer* resource) {
        return reference(resource, GfxAccess::BufferWrite);
    }

    RDGPass& RDGPass::bind(RDGPassCallback callback) {
        m_callback = std::move(callback);
        return *this;
    }

    bool RDGPass::has_resource(RDGResource* r) const {
        return m_referenced.find(r) != m_referenced.end();
    }

}// namespace wmoge
