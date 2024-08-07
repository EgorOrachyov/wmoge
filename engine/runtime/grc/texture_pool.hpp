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

#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_texture.hpp"

#include <mutex>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class TexturePool
     * @brief Pool of preallocated gfx textures (for mem management)
     */
    class TexturePool {
    public:
        TexturePool(GfxDriver& driver);
        ~TexturePool();

        GfxTextureRef allocate(const GfxTextureDesc& desc, const Strid& name = Strid());
        void          release(const GfxTextureRef& handle);

    private:
        // Single item allocated from pool
        struct PoolItem {
            GfxTextureRef texture;
        };

        // Pool configured for a specific type of textures
        struct PoolList {
            std::vector<PoolItem> items;
            std::vector<bool>     items_used;
            std::size_t           n_items      = 0;
            std::size_t           n_items_used = 0;
        };

        PoolList* get_or_create_pool(const GfxTextureDesc& desc);
        PoolList* get_pool(const GfxTextureDesc& desc);

    private:
        using PoolKeyVal = std::pair<GfxTextureDesc, PoolList>;
        using PoolMap    = std::vector<PoolKeyVal>;

        PoolMap    m_pools;
        std::mutex m_mutex;

        GfxDriver& m_driver;
    };

}// namespace wmoge