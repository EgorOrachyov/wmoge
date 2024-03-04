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

#include "resource_manager.hpp"

#include "core/timer.hpp"
#include "debug/profiler.hpp"
#include "event/event_manager.hpp"
#include "event/event_resource.hpp"
#include "system/engine.hpp"

#include "resource/paks/resource_pak_fs.hpp"

#include "resource/loaders/resource_loader_assimp.hpp"
#include "resource/loaders/resource_loader_default.hpp"
#include "resource/loaders/resource_loader_freetype.hpp"
#include "resource/loaders/resource_loader_image.hpp"
#include "resource/loaders/resource_loader_texture.hpp"
#include "resource/loaders/resource_loader_wav.hpp"

#include <chrono>

namespace wmoge {

    ResourceManager::ResourceManager() {
        add_pak(std::make_shared<ResourcePakFileSystem>());
        add_loader(std::make_shared<ResourceLoaderDefault>());
        add_loader(std::make_shared<ResourceLoaderAssimp>());
        add_loader(std::make_shared<ResourceLoaderFreeType>());
        add_loader(std::make_shared<ResourceLoaderImage>());
        add_loader(std::make_shared<ResourceLoaderTexture2d>());
        add_loader(std::make_shared<ResourceLoaderTextureCube>());
        add_loader(std::make_shared<ResourceLoaderWav>());
    }

    AsyncResult<Ref<Resource>> ResourceManager::load_async(const ResourceId& name, ResourceCallback callback) {
        WG_AUTO_PROFILE_RESOURCE("ResourceManager::load_async");

        std::lock_guard guard(m_mutex);

        // already loaded and cached
        Ref<Resource> res = find(name);
        if (res) {
            std::optional<Ref<Resource>> res_opt(res);
            auto                         async_op = make_async_op<Ref<Resource>>();
            async_op->set_result(std::move(res));
            async_op->add_on_completion(std::move(callback));
            return AsyncResult<Ref<Resource>>(async_op);
        }

        // not yet cached, but is loading
        auto loading = m_loading.find(name);
        if (loading != m_loading.end()) {
            auto& async_op = loading->second.async_op;
            async_op->add_on_completion(std::move(callback));
            return AsyncResult<Ref<Resource>>(async_op);
        }

        // try to find meta info, to load from pak
        std::optional<ResourceMeta> resource_meta = find_meta(name);

        if (!resource_meta.has_value()) {
            // failed to load, return dummy async in error state
            auto async_op = make_async_op<Ref<Resource>>();
            async_op->set_failed();
            async_op->add_on_completion(std::move(callback));

            WG_LOG_ERROR("failed to find meta info for " << name);
            return AsyncResult<Ref<Resource>>(async_op);
        }

        // get dependencies which still loading or already loaded
        fast_vector<Async> deps;

        for (const Strid& dep : resource_meta.value().deps) {
            deps.push_back(load_async(dep).as_async());
        }

        // create loading state to track result
        AsyncOp<Ref<Resource>> async_op = make_async_op<Ref<Resource>>();

        // create task to load
        Task task(name, [=, meta = std::move(resource_meta.value())](TaskContext&) {
            Timer timer;
            timer.start();

            Ref<Resource> resource;
            if (meta.loader->load(name, meta, resource)) {
                timer.stop();
                WG_LOG_INFO("load resource " << name << ", time: " << timer.get_elapsed_sec() << " sec");

                if (resource->get_name().empty()) {
                    resource->set_name(name);
                }

                auto event          = make_event<EventResource>();
                event->resource_id  = name;
                event->resource_ref = resource;
                event->notification = ResourceNotification::Loaded;
                Engine::instance()->event_manager()->dispatch_deferred(event);

                std::lock_guard guard(m_mutex);
                m_resources[name] = WeakRef<Resource>(resource);
                async_op->set_result(std::move(resource));
                return 0;
            }
            return 1;
        });

        // schedule to run only if all deps are loaded
        auto task_hnd = task.schedule(Async::join(ArrayView(deps)));

        // add erase of loading state here, since it is possible, that task can be aborted
        task_hnd.add_on_completion([this, name, async_op](AsyncStatus status, std::optional<int>&) {
            std::lock_guard guard(m_mutex);

            if (status == AsyncStatus::Failed) {
                auto event          = make_event<EventResource>();
                event->resource_id  = name;
                event->notification = ResourceNotification::FailedLoad;
                Engine::instance()->event_manager()->dispatch_deferred(event);

                async_op->set_failed();
                WG_LOG_ERROR("failed load resource " << name);
            }

            m_loading.erase(name);
        });

        auto& state    = m_loading[name];
        state.deps     = std::move(deps);
        state.task_hnd = std::move(task_hnd);
        state.async_op = std::move(async_op);

        state.async_op->add_on_completion(std::move(callback));
        return AsyncResult<Ref<Resource>>(state.async_op);
    }

    Ref<Resource> ResourceManager::load(const ResourceId& name) {
        WG_AUTO_PROFILE_RESOURCE("ResourceManager::load");

        Ref<Resource> fast_look_up = find(name);
        if (fast_look_up) {
            return fast_look_up;
        }

        AsyncResult<Ref<Resource>> async = load_async(name);
        async.wait_completed();
        return async.is_ok() ? async.result() : nullptr;
    }

    Ref<Resource> ResourceManager::find(const ResourceId& name) {
        WG_AUTO_PROFILE_RESOURCE("ResourceManager::find");

        std::lock_guard guard(m_mutex);

        auto cached = m_resources.find(name);
        if (cached != m_resources.end()) {
            return cached->second.acquire();
        }

        return {};
    }

    void ResourceManager::add_loader(std::shared_ptr<ResourceLoader> loader) {
        std::lock_guard guard(m_mutex);
        m_loaders[loader->get_name()] = std::move(loader);
    }

    void ResourceManager::add_pak(std::shared_ptr<ResourcePak> pak) {
        std::lock_guard guard(m_mutex);
        m_paks.push_back(std::move(pak));
    }

    std::optional<ResourceLoader*> ResourceManager::find_loader(const Strid& loader) {
        std::lock_guard guard(m_mutex);
        auto            query = m_loaders.find(loader);
        return query != m_loaders.end() ? std::make_optional(query->second.get()) : std::nullopt;
    }

    std::optional<ResourceMeta> ResourceManager::find_meta(const ResourceId& resource) {
        std::lock_guard guard(m_mutex);

        ResourceMeta resource_meta;
        for (auto& pak : m_paks) {
            if (pak->get_meta(resource, resource_meta)) {
                if (resource_meta.cls && resource_meta.loader && resource_meta.pak) {
                    return std::make_optional(std::move(resource_meta));
                }
                if (!resource_meta.cls) {
                    WG_LOG_ERROR("no class found in runtime for " << resource << " in " << pak->get_name());
                }
                if (!resource_meta.loader) {
                    WG_LOG_ERROR("no loader found in runtime for " << resource << " in " << pak->get_name());
                }
                if (!resource_meta.pak) {
                    WG_LOG_ERROR("no pak found in runtime for " << resource << " in " << pak->get_name());
                }
            }
        }

        return std::nullopt;
    }

    void ResourceManager::gc() {
        WG_AUTO_PROFILE_RESOURCE("ResourceManager::gc");

        std::lock_guard guard(m_mutex);
        int             evicted = 0;
        for (auto iter = m_resources.begin(); iter != m_resources.end(); ++iter) {
            if (iter->second.acquire()->refs_count() == 1) {
                iter = m_resources.erase(iter);
                evicted += 1;
            }
        }
        WG_LOG_INFO("gc " << evicted << " unreferenced resources");
    }

    void ResourceManager::clear() {
        WG_AUTO_PROFILE_RESOURCE("ResourceManager::clear");

        std::lock_guard guard(m_mutex);
        m_resources.clear();
    }

}// namespace wmoge