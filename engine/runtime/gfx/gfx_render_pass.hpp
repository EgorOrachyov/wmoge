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

#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_resource.hpp"

#include <array>
#include <utility>

namespace wmoge {

    /**
     * @class GfxRenderPassDesc
     * @brief Gfx rendering pass descriptor
     */
    struct GfxRenderPassDesc {
        GfxRenderPassDesc();
        bool        operator==(const GfxRenderPassDesc& other) const;
        std::size_t hash() const;

        std::array<GfxFormat, GfxLimits::MAX_COLOR_TARGETS> color_target_fmts;// = GfxFormat::Unknown;
        std::array<GfxRtOp, GfxLimits::MAX_COLOR_TARGETS>   color_target_ops; // = GfxRtOp::LoadStore;
        GfxFormat                                           depth_stencil_fmt;// = GfxFormat::Unknown;
        GfxRtOp                                             depth_op;         // = GfxRtOp::LoadStore;
        GfxRtOp                                             stencil_op;       // = GfxRtOp::LoadStore;
    };

    /**
     * @class GfxRenderPassDesc
     * @brief Gfx rendering pass object (for internal usage primary)
     */
    class GfxRenderPass : public GfxResource {
    public:
        ~GfxRenderPass() override                          = default;
        virtual const GfxRenderPassDesc& pass_desc() const = 0;
    };

    using GfxRenderPassRef = Ref<GfxRenderPass>;

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::GfxRenderPassDesc> {
        std::size_t operator()(const wmoge::GfxRenderPassDesc& desc) const {
            return desc.hash();
        }
    };

}// namespace std