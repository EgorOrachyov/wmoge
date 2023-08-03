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

#ifndef WMOGE_HGFX_PASS_HPP
#define WMOGE_HGFX_PASS_HPP

#include "core/object.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"

namespace wmoge {

    /**
     * @brief List of hgfx pass types
     */
    enum class HgfxPassType {
        Default,
    };

    /**
     * @class HgfxPass
     * @brief Base class for any high level graphics logic for in-engine and scriptable rendering
     */
    class HgfxPass : public Object {
    public:
        WG_OBJECT(HgfxPass, Object);

        /**
         * @brief Compiles pass preparing all state required for rendering
         *
         * @note Must be called before any `configure` call
         * @note Must be called outside render pass
         *
         * @param ctx Context to operate on
         * @return True on success
         */
        virtual bool compile(GfxCtx* ctx);

        /**
         * @brief Configures state binding all required resources to gfx
         *
         * @note Must be called only after successful `compile call`
         * @note Must be called inside render pass
         *
         * @param ctx Context to operate on
         * @return True on success
         */
        virtual bool configure(GfxCtx* ctx);

        virtual StringId     get_pass_name();
        virtual HgfxPassType get_pass_type();
    };

}// namespace wmoge

#endif//WMOGE_HGFX_PASS_HPP
