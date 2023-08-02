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

#ifndef WMOGE_ECS_ENTITY_HPP
#define WMOGE_ECS_ENTITY_HPP

#include "ecs/ecs_core.hpp"

#include <cinttypes>
#include <functional>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>

namespace wmoge {

    /**
     * @brief State on an entity, tracked by a ecs world
     */
    enum class EcsEntityState : std::uint8_t {
        Dead = 0,
        Alive
    };

    /**
     * @class EcsEntityInfo
     * @brief Associated entity info stored in a world
     */
    struct EcsEntityInfo {
        EcsEntityState state   = EcsEntityState::Dead;
        std::uint16_t  arch    = EcsLimits::MAX_ARCHS;
        std::uint32_t  storage = EcsLimits::MAX_ENTITIES_PER_ARCH;
        std::uint32_t  gen     = EcsLimits::MAX_GENERATIONS_PER_ARC;
    };

    static_assert(sizeof(EcsEntityInfo) <= 3 * sizeof(std::uint32_t), "Must fit 3 32bit words");

    /**
     * @class EcsEntity
     * @brief Handle for a ecs entity
     */
    struct EcsEntity {
        union {
            struct {
                std::uint32_t idx;
                std::uint32_t gen;
            };
            std::uint64_t value = std::numeric_limits<std::uint64_t>::max();
        };

        EcsEntity() = default;

        EcsEntity(std::uint32_t idx, std::uint32_t gen)
            : idx(idx), gen(gen) {}

        bool operator==(const EcsEntity& other) const { return value == other.value; }
        bool operator!=(const EcsEntity& other) const { return value != other.value; }

        [[nodiscard]] bool is_valid() const { return value != std::numeric_limits<std::uint64_t>::max(); }
        [[nodiscard]] bool is_invalid() const { return !is_valid(); }

        [[nodiscard]] std::string to_string() const {
            if (is_invalid()) {
                return "'null'";
            }

            std::stringstream stream;
            stream << "\'"
                   << "idx=" << idx
                   << ",gen=" << gen
                   << "\'";

            return stream.str();
        }
    };

    static_assert(sizeof(EcsEntity) == sizeof(std::uint64_t), "Entity handle must fit 64bit-word exactly");
    static_assert(std::is_trivially_destructible_v<EcsEntity>, "Entity handle must be trivial as ptr on int");

    inline std::ostream& operator<<(std::ostream& stream, const EcsEntity& entity) {
        stream << entity.to_string();
        return stream;
    }

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::EcsEntity> {
    public:
        std::size_t operator()(const wmoge::EcsEntity& entity) const {
            return std::hash<std::uint64_t>()(entity.value);
        }
    };

}// namespace std

#endif//WMOGE_ECS_ENTITY_HPP
