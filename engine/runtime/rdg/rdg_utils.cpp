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

#include "rdg_utils.hpp"

#include "gfx/gfx_driver.hpp"
#include "grc/shader_manager.hpp"
#include "rdg/rdg_profiling.hpp"

namespace wmoge {

    void RdgUtils::update_buffer(RdgGraph& graph, const Strid& name, RdgVertBuffer* buffer, int offset, array_view<const std::uint8_t> data) {
        WG_PROFILE_RDG_SCOPE("RdgUtils::update_buffer", graph);
        auto data_capture = graph.make_upload_data(data);
        graph.add_copy_pass(name, {})
                .copy_destination(buffer)
                .bind([=](RdgPassContext& context) {
                    context.update_vert_buffer(buffer->get_buffer(), offset, static_cast<int>(data.size()), {data_capture->buffer(), data_capture->size()});
                    return WG_OK;
                });
    }

    void RdgUtils::update_buffer(RdgGraph& graph, const Strid& name, RdgIndexBuffer* buffer, int offset, array_view<const std::uint8_t> data) {
        WG_PROFILE_RDG_SCOPE("RdgUtils::update_buffer", graph);
        auto data_capture = graph.make_upload_data(data);
        graph.add_copy_pass(name, {})
                .copy_destination(buffer)
                .bind([=](RdgPassContext& context) {
                    context.update_index_buffer(buffer->get_buffer(), offset, static_cast<int>(data.size()), {data_capture->buffer(), data_capture->size()});
                    return WG_OK;
                });
    }

    void RdgUtils::update_buffer(RdgGraph& graph, const Strid& name, RdgStorageBuffer* buffer, int offset, array_view<const std::uint8_t> data) {
        WG_PROFILE_RDG_SCOPE("RdgUtils::update_buffer", graph);
        auto data_capture = graph.make_upload_data(data);
        graph.add_copy_pass(name, {})
                .copy_destination(buffer)
                .bind([=](RdgPassContext& context) {
                    context.update_storage_buffer(buffer->get_buffer(), offset, static_cast<int>(data.size()), {data_capture->buffer(), data_capture->size()});
                    return WG_OK;
                });
    }

}// namespace wmoge