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

#include "io/archive.hpp"
#include "io/yaml.hpp"

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

        friend Status yaml_read(YamlConstNodeRef node, EcsArch& arch) {
            return yaml_read(node, *((EcsArch::Bitset*) &arch));
        }
        friend Status yaml_write(YamlNodeRef node, const EcsArch& arch) {
            return yaml_write(node, *((const EcsArch::Bitset*) &arch));
        }
        friend Status archive_read(Archive& archive, EcsArch& arch) {
            return archive_read(archive, *((EcsArch::Bitset*) &arch));
        }
        friend Status archive_write(Archive& archive, const EcsArch& arch) {
            return archive_write(archive, *((const EcsArch::Bitset*) &arch));
        }
    };

    inline std::ostream& operator<<(std::ostream& stream, const EcsArch& arch) {
        stream << arch.to_string();
        return stream;
    }

    /**
     * @class EcsQuery
     * @brief Describes read-write dependencies of an system query for processing
     */
    struct EcsQuery {
        std::bitset<EcsLimits::MAX_COMPONENTS> read;
        std::bitset<EcsLimits::MAX_COMPONENTS> write;

        EcsQuery() = default;

        template<typename Component>
        EcsQuery& set_read() {
            read.set(Component::IDX);
            return *this;
        }

        template<typename Component>
        EcsQuery& set_write() {
            read.set(Component::IDX);
            write.set(Component::IDX);
            return *this;
        }

        template<typename Component>
        [[nodiscard]] bool has_read() const {
            return read.test(Component::IDX);
        }

        template<typename Component>
        [[nodiscard]] bool has_write() const {
            return write.test(Component::IDX);
        }

        [[nodiscard]] std::bitset<EcsLimits::MAX_COMPONENTS> affected() const {
            return read | write;
        }

        [[nodiscard]] std::string to_string() const;
    };

    inline std::ostream& operator<<(std::ostream& stream, const EcsQuery& query) {
        stream << query.to_string();
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