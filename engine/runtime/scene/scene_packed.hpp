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

#include "asset/asset.hpp"
#include "core/async.hpp"
#include "io/tree.hpp"
#include "scene/scene.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class ScenePacked
     * @brief Represents packed scene asset which can be used to load scenes
     *
     * Packed scene stores serialized scene description. Supported formats are:
     * text format based on yaml document, easy to write, read and parse. Scene
     * representation loaded and saved in a pack. This serialized representation
     * used to instantiate scene.
     *
     * Scene can be instantiated synchronously or asynchronously by async.
     * For instantiation pack constructs and async task graph, so actual scene
     * creating starts when deps are ready. Everything is done in task manager.
     *
     * @see Scene
     */
    class ScenePacked final : public Asset {
    public:
        WG_RTTI_CLASS(ScenePacked, Asset);

        ScenePacked()           = default;
        ~ScenePacked() override = default;

    private:
        SceneData m_data;
    };

    WG_RTTI_CLASS_BEGIN(ScenePacked) {
        WG_RTTI_META_DATA(RttiUiHint(""));
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge