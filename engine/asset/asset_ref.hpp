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

#include "asset/asset.hpp"
#include "asset/asset_manager.hpp"
#include "io/archive.hpp"
#include "io/yaml.hpp"
#include "system/engine.hpp"

#include <cassert>
#include <optional>
#include <type_traits>

namespace wmoge {

    /**
     * @class AssetRef
     * @brief Aux box to store asset ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class AssetRef : public Ref<T> {
    public:
        static_assert(std::is_base_of_v<Asset, T>, "Must be a asset");

        AssetRef() = default;
        AssetRef(Ref<T> ptr) : Ref<T>(std::move(ptr)) {}
    };

    /**
     * @class AssetRefWeak
     * @brief Aux box to store asset ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class AssetRefWeak : public AssetId {
    public:
        static_assert(std::is_base_of_v<Asset, T>, "Must be a asset");

        AssetRefWeak() = default;
        AssetRefWeak(const AssetId& id) : AssetId(id) {}
        AssetRefWeak(const AssetRef<T>& ref) : AssetId(ref ? ref->get_id() : AssetId()) {}
    };

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, AssetRef<T>& ref) {
        AssetId id;
        WG_YAML_READ(node, id);
        Ref<T> ptr = Engine::instance()->asset_manager()->load(id).cast<T>();
        if (!ptr) {
            return StatusCode::NoAsset;
        }
        ref = AssetRef<T>(ptr);
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const AssetRef<T>& ref) {
        assert(ref);
        if (!ref) {
            return StatusCode::NoAsset;
        }
        WG_YAML_WRITE(node, ref->get_id());
        return StatusCode::Ok;
    }

    template<typename T>
    Status archive_read(Archive& archive, AssetRef<T>& ref) {
        AssetId id;
        WG_ARCHIVE_READ(archive, id);
        Ref<T> ptr = Engine::instance()->asset_manager()->load(id).cast<T>();
        if (!ptr) {
            return StatusCode::NoAsset;
        }
        ref = AssetRef<T>(ptr);
        return StatusCode::Ok;
    }

    template<typename T>
    Status archive_write(Archive& archive, const AssetRef<T>& ref) {
        assert(ref);
        if (!ref) {
            return StatusCode::NoAsset;
        }
        WG_ARCHIVE_WRITE(archive, ref->get_id());
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, AssetRefWeak<T>& ref) {
        AssetId id;
        WG_YAML_READ(node, id);
        ref = AssetRefWeak<T>(id);
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const AssetRefWeak<T>& ref) {
        AssetId id = ref;
        WG_YAML_WRITE(node, id);
        return StatusCode::Ok;
    }

    template<typename T>
    Status archive_read(Archive& archive, AssetRefWeak<T>& ref) {
        AssetId id;
        WG_ARCHIVE_READ(archive, id);
        ref = AssetRefWeak<T>(id);
        return StatusCode::Ok;
    }

    template<typename T>
    Status archive_write(Archive& archive, const AssetRefWeak<T>& ref) {
        AssetId id = ref;
        WG_ARCHIVE_WRITE(archive, id);
        return StatusCode::Ok;
    }

}// namespace wmoge