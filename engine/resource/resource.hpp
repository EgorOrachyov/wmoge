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

#ifndef WMOGE_RESOURCE_HPP
#define WMOGE_RESOURCE_HPP

#include "core/class.hpp"
#include "core/fast_set.hpp"
#include "core/fast_vector.hpp"
#include "core/object.hpp"
#include "core/string_id.hpp"
#include "core/uuid.hpp"
#include "core/weak_ref.hpp"
#include "event/event.hpp"
#include "io/serialization.hpp"

#include <cstddef>
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

namespace wmoge {

    /**
     * @class ResourceId 
     * @brief Class to track and access resource by its id
    */
    class ResourceId {
    public:
        ResourceId() = default;
        ResourceId(const std::string& id);
        ResourceId(const Strid& id);

        bool operator==(const ResourceId& other) const { return m_name == other.m_name; }
        bool operator!=(const ResourceId& other) const { return m_name != other.m_name; }
        bool operator<(const ResourceId& other) const { return m_name < other.m_name; }

        operator bool() const { return is_empty(); }
        operator Strid() const { return sid(); }

        [[nodiscard]] const Strid&       sid() const { return m_name; }
        [[nodiscard]] const std::string& str() const { return m_name.str(); }
        [[nodiscard]] bool               is_empty() const { return m_name.empty(); }
        [[nodiscard]] std::size_t        hash() const { return m_name.hash(); }

        friend Status yaml_read(const YamlConstNodeRef& node, ResourceId& id);
        friend Status yaml_write(YamlNodeRef node, const ResourceId& id);
        friend Status archive_read(Archive& archive, ResourceId& id);
        friend Status archive_write(Archive& archive, const ResourceId& id);

    private:
        Strid m_name;
    };

    static_assert(std::is_trivially_destructible_v<ResourceId>, "id must be trivial as ptr or int");

    inline std::ostream& operator<<(std::ostream& stream, const ResourceId& id) {
        stream << id.sid();
        return stream;
    }

    /**
     * @class Resource
     * @brief Base class for any engine resource
    */
    class Resource : public WeakRefCnt<Object> {
    public:
        WG_OBJECT(Resource, Object);

        void              set_name(Strid name) { m_id = ResourceId(name); }
        void              set_id(ResourceId id) { m_id = id; }
        void              set_uuid(UUID uuid) { m_uuid = uuid; }
        const Strid&      get_name() { return m_id.sid(); }
        const ResourceId& get_id() { return m_id; }
        const UUID&       get_uuid() { return m_uuid; }

        virtual void collect_deps(class ResourceDependencies& deps) {}

        Status copy_to(Object& other) const override;
        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status write_to_yaml(YamlNodeRef node) const override;

    private:
        ResourceId m_id;
        UUID       m_uuid;
    };

    /**
     * @class ResourceDependencies
     * @brief Class to collect dependencies of a particular resource (primary editor only feature)
    */
    class ResourceDependencies {
    public:
        ResourceDependencies()  = default;
        ~ResourceDependencies() = default;

        /** @brief Mode how to collect deps */
        enum class CollectionMode {
            OneLevel,
            MultipleLevels,
            FullDepth
        };

        void set_mode(CollectionMode mode, std::optional<int> num_levels);
        void add(const Ref<Resource>& resource);

        [[nodiscard]] CollectionMode             get_mode() const { return m_mode; }
        [[nodiscard]] fast_vector<Ref<Resource>> to_vector() const;

    private:
        fast_set<Ref<class Resource>> m_resources;
        int                           m_max_depth = 1;
        int                           m_cur_depth = 0;
        CollectionMode                m_mode      = CollectionMode::OneLevel;
    };

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::ResourceId> {
    public:
        std::size_t operator()(const wmoge::ResourceId& id) const {
            return id.hash();
        }
    };

}// namespace std

#endif//WMOGE_RESOURCE_HPP
