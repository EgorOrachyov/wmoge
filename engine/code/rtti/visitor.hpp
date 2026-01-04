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

#include "rtti/builtin.hpp"
#include "rtti/class.hpp"
#include "rtti/enum.hpp"
#include "rtti/struct.hpp"

namespace wmoge {

    /**
     * @class RttiVisitor
     * @brief Top to bottom dfs-like visitor to inspect rtti objects
     */
    class RttiVisitor {
    public:
        virtual ~RttiVisitor() = default;

        Status visit(const RttiType* rtti, std::uint8_t* src);

    protected:
        virtual Status visit_optional(const RttiTypeOptional* rtti, std::uint8_t* src);
        virtual Status visit_pair(const RttiTypePair* rtti, std::uint8_t* src);
        virtual Status visit_struct(const RttiStruct* rtti, std::uint8_t* src);
        virtual Status visit_class(const RttiClass* rtti, std::uint8_t* src);
        virtual Status visit_vector(const RttiTypeVector* rtti, std::uint8_t* src);
        virtual Status visit_set(const RttiTypeSet* rtti, std::uint8_t* src);
        virtual Status visit_map(const RttiTypeMap* rtti, std::uint8_t* src);
        virtual Status visit_ref(const RttiTypeRef* rtti, std::uint8_t* src);
        virtual Status visit_asset_ref(const RttiTypeAssetRef* rtti, std::uint8_t* src);
        virtual Status visit_vec(const RttiTypeVec* rtti, std::uint8_t* src);
        virtual Status visit_mask(const RttiTypeMask* rtti, std::uint8_t* src);
        virtual Status visit_bitset(const RttiTypeBitset* rtti, std::uint8_t* src);
        virtual Status visit_function(const RttiTypeFunction* rtti, std::uint8_t* src);
        virtual Status visit_enum(const RttiEnum* rtti, std::uint8_t* src);
        virtual Status visit_fundamental(const RttiType* rtti, std::uint8_t* src);
    };

}// namespace wmoge