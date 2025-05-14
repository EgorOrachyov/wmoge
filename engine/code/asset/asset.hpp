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

#include "asset/asset_flags.hpp"
#include "asset/asset_id.hpp"
#include "core/weak_ref.hpp"
#include "rtti/traits.hpp"

namespace wmoge {

    /**
     * @class Asset
     * @brief Base class for any engine asset
    */
    class Asset : public WeakRefCnt<Asset, RttiObject> {
    public:
        WG_RTTI_CLASS(Asset, RttiObject);

        void              set_id(AssetId id) { m_id = id; }
        void              set_flags(AssetFlags flags) { m_flags = flags; }
        const AssetId&    get_id() { return m_id; }
        const AssetFlags& get_flags() const { return m_flags; }

    private:
        AssetId    m_id;
        AssetFlags m_flags;
    };

    WG_RTTI_CLASS_BEGIN(Asset) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_id, {RttiNoSaveLoad});
        WG_RTTI_FIELD(m_flags, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

}// namespace wmoge