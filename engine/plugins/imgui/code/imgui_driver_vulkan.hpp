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
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "imgui_driver.hpp"
#include "platform/window.hpp"

namespace wmoge {

    /**
     * @class ImguiDriverVulkan
     * @brief Driver implementation for vulkan imgui driver
     */
    class ImguiDriverVulkan : public ImguiDriver {
    public:
        ImguiDriverVulkan(const Ref<Window>& window, GfxDriver* driver);
        ~ImguiDriverVulkan() override;

        void        new_frame(std::size_t frame_id) override;
        void        render(RdgGraph& graph, RdgTexture* target) override;
        ImTextureID get_texture_id(const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) override;

    private:
        void textures_clear();
        void textures_gc();

    private:
        struct TextureInfo {
            std::size_t last_frame_used = 0;
            ImTextureID id;
        };

        GfxRenderPassRef                       m_render_pass;
        Ref<Window>                            m_window;
        flat_map<Ref<GfxTexture>, TextureInfo> m_texture_ids;
        std::size_t                            m_frame_id = 0;
    };

}// namespace wmoge