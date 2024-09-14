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

#include "io/property_tree.hpp"
#include "io/stream.hpp"

#include <bitset>
#include <cinttypes>
#include <functional>
#include <initializer_list>
#include <limits>
#include <ostream>
#include <string>

namespace wmoge {

    /** @brief Ecs limits */
    struct EcsLimits {
        /** @brief Max num of unique components in ecs system */
        static constexpr int MAX_COMPONENTS = 64;
        /** @brief Max num of unique archetypes */
        static constexpr int MAX_ARCHS = 2000;
        /** @brief Max num of unique entities per giver archetype */
        static constexpr int MAX_ENTITIES_PER_ARCH = 1u << 24u;
        /** @brief Max num of generations all entities can live */
        static constexpr int MAX_GENERATIONS_PER_ARC = 1u << 24u;
    };

    /**
     * @class EcsArch
     * @brief An entity archetype defining its component structure
     */
    struct EcsArch final : public std::bitset<EcsLimits::MAX_COMPONENTS> {
        using Bitset = std::bitset<EcsLimits::MAX_COMPONENTS>;

        EcsArch() = default;
        EcsArch(const Bitset& b) : Bitset(b) {}

        template<typename Component>
        void set_component() { set(Component::IDX); }

        template<typename Component>
        [[nodiscard]] bool has_component() const { return test(Component::IDX); }

        template<typename Func>
        void for_each_component(Func&& func) const {
            for (int i = 0; i < EcsLimits::MAX_COMPONENTS; i++) {
                if (test(i)) {
                    func(i);
                }
            }
        }

        [[nodiscard]] std::string to_string() const;

        friend Status tree_read(IoContext& context, IoPropertyTree& tree, EcsArch& arch) {
            return tree_read(context, tree, *((EcsArch::Bitset*) &arch));
        }
        friend Status tree_write(IoContext& context, IoPropertyTree& tree, const EcsArch& arch) {
            return tree_write(context, tree, *((const EcsArch::Bitset*) &arch));
        }
        friend Status stream_read(IoContext& context, IoStream& stream, EcsArch& arch) {
            return stream_read(context, stream, *((EcsArch::Bitset*) &arch));
        }
        friend Status stream_write(IoContext& context, IoStream& stream, const EcsArch& arch) {
            return stream_write(context, stream, *((const EcsArch::Bitset*) &arch));
        }
    };

    inline std::ostream& operator<<(std::ostream& stream, const EcsArch& arch) {
        stream << arch.to_string();
        return stream;
    }

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::EcsArch> {
    public:
        std::size_t operator()(const wmoge::EcsArch& arch) const {
            return std::hash<std::bitset<wmoge::EcsLimits::MAX_COMPONENTS>>()(arch);
        }
    };

}// namespace std