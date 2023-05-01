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

#ifndef WMOGE_PFX_RENDERER_HPP
#define WMOGE_PFX_RENDERER_HPP

#include "render/render_object.hpp"
#include "render/render_view.hpp"

namespace wmoge {

    /**
     * @class PfxComponentRenderData
     * @brief Data supplied to rendered to draw cpu particles
     */
    struct PfxComponentRenderData {
        ref_ptr<Data> vertices;
        ref_ptr<Data> indices;
        int           n_vertices = 0;
        int           n_indices  = 0;
    };

    /**
     * @class PfxRenderer
     * @brief Encapsulates logic to render a pfx particles
     */
    class PfxRenderer {
    public:
        virtual ~PfxRenderer() = default;

        virtual void on_update_data(const PfxComponentRenderData& data) {}
        virtual void on_render_dynamic(RenderViewList& views, const RenderViewMask& mask, RenderObject* object) {}
        virtual bool need_render_dynamic() const { return false; }
    };

}// namespace wmoge

#endif//WMOGE_PFX_RENDERER_HPP
