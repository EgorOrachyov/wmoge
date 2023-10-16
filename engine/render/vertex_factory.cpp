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

#include "vertex_factory.hpp"

#include "core/engine.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/enum.hpp"

namespace wmoge {

    void VertexFactory::init() {
        GfxDriver* gfx_driver = Engine::instance()->gfx_driver();

        const VertexInputType input_types[] = {
                VertexInputType::Default,
                VertexInputType::PositionOnly};

        for (VertexInputType input_type : input_types) {
            int             used_buffers = 0;
            GfxVertElements elemets;
            fill_elements(input_type, elemets, used_buffers);
            cache_vert_format(gfx_driver, elemets, input_type);
        }
    }

    void VertexFactory::cache_vert_format(class GfxDriver* driver, const GfxVertElements& elements, VertexInputType input_type) {
        const int      idx        = int(input_type);
        const StringId debug_name = SID(get_friendly_name() + " " + Enum::to_str(input_type));
        m_gfx_formats[idx]        = driver->make_vert_format(elements, debug_name);
    }

    const VertexFactoryType& VertexFactory::get_type_info() const {
        static VertexFactoryType s_type = {
                SID("VertexFactory"),
                false};

        return s_type;
    }

}// namespace wmoge