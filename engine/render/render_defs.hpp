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

#ifndef WMOGE_RENDER_DEFS_HPP
#define WMOGE_RENDER_DEFS_HPP

#include <bitset>

namespace wmoge {

    /**
     * @brief Type of projection for cameras
     */
    enum class CameraProjection {
        Perspective = 0,
        Orthographic
    };

    /**
     * @brief Type of camera, how and what to render
     */
    enum class CameraType {
        Color  = 0,
        Shadow = 1,
        Image  = 2,
        Debug  = 3,
        Editor = 4
    };

    /** 
     * @class RenderLimits
     * @brief Holds global render engine config
     */
    struct RenderLimits {
        static constexpr int MAX_CAMERAS = 64;
        static constexpr int MAX_VIEWS   = MAX_CAMERAS;
    };

    /**
     * @class RenderCameraMask
     * @brief Mask of toggled cameras, can be used for filtering and culling
     */
    class RenderCameraMask : public std::bitset<RenderLimits::MAX_CAMERAS> {};

}// namespace wmoge

#endif//WMOGE_RENDER_DEFS_HPP