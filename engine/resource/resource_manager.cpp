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

#include "core/engine.hpp"
#include "core/timer.hpp"
#include "debug/profiler.hpp"
#include "event/event_manager.hpp"
#include "event/event_resource.hpp"
#include "resource/loaders/resource_loader_assimp.hpp"
#include "resource/loaders/resource_loader_default.hpp"
#include "resource/paks/resource_pak_fs.hpp"

#include <chrono>

namespace wmoge {

    ResourceManager::ResourceManager() {
        add_pak(std::make_shared<ResourcePakFileSystem>());
        add_loader(std::make_shared<ResourceLoaderDefault>());
        add_loader(std::make_shared<ResourceLoaderAssimp>());
    }

    Async<ref_ptr<Resource>> ResourceManager::load_async(const StringId& name, ResourceCallback callback) {
        WG_AUTO_PROFILE_RESOURCE();

        std::lock_guard guard(m_mutex);

        // already loaded and cached
        auto cached = m_resources.find(name);
        if (cached != m_resources.end()) {
            ref_ptr<Resource>                res(cached->second);
            std::optional<ref_ptr<Resource>> res_opt(res);
            auto                             async_op = make_async_op<ref_ptr<Resource>>();
            async_op->set_result(std::move(res));
            async_op->add_on_completion(std::move(callback));
            return {async_op};
        }

        // not yet cached, but is loading
        auto loading = m_loading.find(name);
        if (loading != m_loading.end()) {
            auto& async_op = loading->second.async_op;
            async_op->add_on_completion(std::move(callback));
            return {async_op};
        }

        ResourceMeta                    resource_meta;
        std::shared_ptr<ResourceLoader> resource_loader;

        if (load_meta(name, resource_meta, resource_loader)) {
            if (LoadState* loading_task = load_internal(name, resource_meta, resource_loader)) {
                loading_task->async_op->add_on_completion(std::move(callback));
                return {loading_task->async_op};
            }
        }

        auto async_op = make_async_op<ref_ptr<Resource>>();
        async_op->set_failed();
        async_op->add_on_completion(std::move(callback));
        return {async_op};
    }

    ref_ptr<Resource> ResourceManager::load(const StringId& name) {
        WG_AUTO_PROFILE_RESOURCE();

        Async<ref_ptr<Resource>> async = load_async(name);
        async.wait_completed();
        return async.is_ok() ? async.result() : nullptr;
    }

    ref_ptr<Resource> ResourceManager::find(const StringId& name) {
        WG_AUTO_PROFILE_RESOURCE();

        std::lock_guard guard(m_mutex);

        auto cached = m_resources.find(name);
        if (cached != m_resources.end()) {
            return cached->second;
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
    void ResourceManager::gc() {
        WG_AUTO_PROFILE_RESOURCE();

        std::lock_guard guard(m_mutex);
        int             evicted = 0;
        for (auto iter = m_resources.begin(); iter != m_resources.end(); ++iter) {
            if (iter->second->refs_count() == 1) {
                iter = m_resources.erase(iter);
                evicted += 1;
            }
        }
        WG_LOG_INFO("gc " << evicted << " unreferenced resources");
    }
    void ResourceManager::clear() {
        WG_AUTO_PROFILE_RESOURCE();
        std::lock_guard guard(m_mutex);
        m_resources.clear();
    }

    bool ResourceManager::load_meta(const StringId& name, ResourceMeta& resource_meta, std::shared_ptr<ResourceLoader>& loader) {
        WG_AUTO_PROFILE_RESOURCE();

        // look for a resource info about resource in paks
        for (auto& pak : m_paks) {
            if (pak->meta(name, resource_meta)) {
                break;
            }
        }

        // no info about resource, cannot load
        if (!resource_meta.resource_class) {
            WG_LOG_ERROR("failed to find resource in paks: " << name);
            return false;
        }

        // find loader
        auto loader_iter = m_loaders.find(resource_meta.loader);
        if (loader_iter == m_loaders.end()) {
            WG_LOG_ERROR("failed to find loader for " << name);
            return false;
        }

        loader = loader_iter->second;
        return true;
    }

    ResourceManager::LoadState* ResourceManager::load_internal(const StringId& name, ResourceMeta& resource_meta, const std::shared_ptr<ResourceLoader>& loader) {
        WG_AUTO_PROFILE_RESOURCE();

        // get dependencies which still loading or already loaded
        fast_vector<ref_ptr<Resource>>          deps_res;
        fast_vector<AsyncOp<ref_ptr<Resource>>> deps_ops;
        fast_vector<ref_ptr<Task>>              deps_tasks;

        if (!resource_meta.deps.empty()) {
            for (const StringId& dep : resource_meta.deps) {
                if (ref_ptr<Resource> found = find(dep)) {
                    deps_res.push_back(std::move(found));
                    continue;
                }
                ResourceMeta                    dep_meta;
                std::shared_ptr<ResourceLoader> dep_loader;
                if (load_meta(dep, dep_meta, dep_loader)) {
                    if (LoadState* state = load_internal(dep, dep_meta, dep_loader)) {
                        deps_ops.push_back(state->async_op);
                        deps_tasks.push_back(state->task);
                        continue;
                    }
                }
                return nullptr;
            }
        }

        // create loading task
        AsyncOp<ref_ptr<Resource>> async_op = make_async_op<ref_ptr<Resource>>();

        ref_ptr<Task> task = make_ref<Task>(name, [=, meta = std::move(resource_meta)](TaskContext&) {
            Timer timer;
            timer.start();

            ref_ptr<Resource> resource;
            if (loader->load(name, meta, resource)) {
                timer.stop();
                WG_LOG_INFO("load resource " << name << ", time: " << timer.get_elapsed_sec() << " sec");

                auto event          = make_event<EventResource>();
                event->resource_id  = name;
                event->resource_ref = resource;
                event->notification = ResourceNotification::Loaded;
                Engine::instance()->event_manager()->dispatch(event);

                std::lock_guard guard(m_mutex);
                m_resources[name] = resource;
                async_op->set_result(std::move(resource));
                return 0;
            }
            return 1;
        });

        task->add_on_completion([this, name, async_op](AsyncStatus status, std::optional<int>&) {
            std::lock_guard guard(m_mutex);
            if (status == AsyncStatus::Failed) {
                auto event          = make_event<EventResource>();
                event->resource_id  = name;
                event->notification = ResourceNotification::FailedLoad;
                Engine::instance()->event_manager()->dispatch(event);

                async_op->set_failed();
                WG_LOG_ERROR("failed load resource " << name);
            }
            m_loading.erase(name);
        });

        for (auto& t : deps_tasks) {
            t->add_to_notify(task);
        }

        task->run();

        auto& state    = m_loading[name];
        state.deps_res = std::move(deps_res);
        state.deps_ops = std::move(deps_ops);
        state.task     = std::move(task);
        state.async_op = std::move(async_op);

        return &state;
    }

}// namespace wmoge