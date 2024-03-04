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

#ifndef WMOGE_GFX_DESC_SET_HPP
#define WMOGE_GFX_DESC_SET_HPP

#include "core/crc32.hpp"
#include "core/fast_vector.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_resource.hpp"
#include "gfx/gfx_sampler.hpp"
#include "io/serialization.hpp"

#include <array>
#include <cinttypes>
#include <functional>
#include <utility>

namespace wmoge {

    /**
     * @class GfxDescBinging
     * @brief Describes single resource binding in a layout
     */
    struct GfxDescBinging {
        GfxBindingType type    = GfxBindingType::Unknown;
        std::int16_t   binding = 0;
        std::int16_t   count   = 1;
        Strid          name;

        [[nodiscard]] bool        operator==(const GfxDescBinging& other) const;
        [[nodiscard]] bool        operator!=(const GfxDescBinging& other) const;
        [[nodiscard]] std::size_t hash() const;

        WG_IO_DECLARE(GfxDescBinging);
    };

    /**
     * @class GfxDescBindPoint
     * @brief Key of a single binding to a pipeline
     */
    struct GfxDescBindPoint {
        GfxBindingType type          = GfxBindingType::Unknown;
        std::int16_t   binding       = 0;
        std::int16_t   array_element = 0;

        [[nodiscard]] bool        operator==(const GfxDescBindPoint& other) const;
        [[nodiscard]] bool        operator!=(const GfxDescBindPoint& other) const;
        [[nodiscard]] std::size_t hash() const;
    };

    /*
     * @class GfxDescBindValue
     * @brief Binding value with required data for bind
     */
    struct GfxDescBindValue {
        Ref<GfxResource> resource;
        Ref<GfxSampler>  sampler;
        int              offset = 0;
        int              range  = 0;
    };

    /** @brief Array of res desc for layout creation */
    using GfxDescSetLayoutDesc = fast_vector<GfxDescBinging, 8>;

    /** @brief Single resource description in a set */
    using GfxDescSetResource = std::pair<GfxDescBindPoint, GfxDescBindValue>;

    /** @brief Array of resources with bind points and values for desc set creation */
    using GfxDescSetResources = fast_vector<GfxDescSetResource, 8>;

    /**
     * @class GfxDescSetLayout
     * @brief Layout of a single resources set for pipeline creation
     */
    class GfxDescSetLayout : public GfxResource {
    public:
        ~GfxDescSetLayout() override = default;

        [[nodiscard]] std::size_t                 size() const { return m_desc.size(); }
        [[nodiscard]] const GfxDescBinging&       binding(int at) const { return m_desc[at]; }
        [[nodiscard]] const GfxDescSetLayoutDesc& desc() const { return m_desc; }

    protected:
        GfxDescSetLayoutDesc m_desc;
    };

    /**
     * @class GfxDescSet
     * @brief Represent set of resources optimized for binding to a pipeline
     */
    class GfxDescSet : public GfxResource {
    public:
        ~GfxDescSet() override = default;

        [[nodiscard]] std::size_t             size() const { return m_resources.size(); }
        [[nodiscard]] const GfxDescBindPoint& point(int at) const { return m_resources[at].first; }
        [[nodiscard]] const GfxDescBindValue& value(int at) const { return m_resources[at].second; }

        static void fill_required_layout(const GfxDescSetResources& resources, GfxDescSetLayoutDesc& layout);

    protected:
        GfxDescSetResources m_resources;
    };

    /** @brief Array of layouts descriptions for shader and pipeline creation */
    using GfxDescSetLayoutDescs = fast_vector<GfxDescSetLayoutDesc, GfxLimits::MAX_DESC_SETS>;

    /** @brief Array of layouts for shader and pipeline creation */
    using GfxDescSetLayouts = fast_vector<Ref<GfxDescSetLayout>, GfxLimits::MAX_DESC_SETS>;

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::GfxDescBinging> {
    public:
        std::size_t operator()(const wmoge::GfxDescBinging& binging) const {
            return binging.hash();
        }
    };

    template<>
    struct hash<wmoge::GfxDescBindPoint> {
    public:
        std::size_t operator()(const wmoge::GfxDescBindPoint& point) const {
            return point.hash();
        }
    };

    template<>
    struct hash<wmoge::GfxDescSetLayoutDesc> {
    public:
        std::size_t operator()(const wmoge::GfxDescSetLayoutDesc& desc) const {
            std::hash<wmoge::GfxDescBinging> hasher;
            std::size_t                      hash = 0xffffffffff;

            for (const auto& binding : desc) {
                hash ^= hasher(binding);
            }

            return hash;
        }
    };

}// namespace std

#endif//WMOGE_GFX_DESC_SET_HPP
