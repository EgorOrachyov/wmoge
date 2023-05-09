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

#ifndef WMOGE_GFX_DRIVER_THREADED_HPP
#define WMOGE_GFX_DRIVER_THREADED_HPP

#include "core/cmd_stream.hpp"
#include "gfx/gfx_driver.hpp"

namespace wmoge {

    /**
     * @class GfxDriverThreaded
     * @brief An extension for a gfx driver for threaded communication support
     *
     * This interface extends gfx driver, so created resources are only constructed
     * objects by gfx driver factory. Actual initialization is done only on gfx thread.
     *
     * @see GfxDriver
     * @see GfxDriverWrapper
     */
    class GfxDriverThreaded : public GfxDriver {
    public:
        ~GfxDriverThreaded() override = default;

        virtual CmdStream* cmd_stream() = 0;
    };

}// namespace wmoge

#endif//WMOGE_GFX_DRIVER_THREADED_HPP
