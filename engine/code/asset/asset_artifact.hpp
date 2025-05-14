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

#include "core/sha256.hpp"
#include "core/uuid.hpp"
#include "rtti/traits.hpp"
#include "rtti/type_ref.hpp"

namespace wmoge {

    /**
     * @class AssetArtifact
     * @brief Asset artifact derived through import process
     */
    struct AssetArtifact {
        WG_RTTI_STRUCT(AssetArtifact)

        Sha256       hash;
        std::size_t  size;
        UUID         uuid;
        RttiRefClass cls;
    };

    WG_RTTI_STRUCT_BEGIN(AssetArtifact) {
        WG_RTTI_FIELD(hash, {});
        WG_RTTI_FIELD(size, {});
        WG_RTTI_FIELD(uuid, {});
        WG_RTTI_FIELD(cls, {});
    }
    WG_RTTI_END;

}// namespace wmoge