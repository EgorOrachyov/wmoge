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
#include "asset/asset_db.hpp"
#include "asset/asset_import_settings.hpp"
#include "asset/asset_importer.hpp"
#include "asset/asset_library.hpp"
#include "asset/asset_loader.hpp"
#include "asset/asset_manager.hpp"
#include "asset/asset_ref.hpp"

#include "audio/audio_bus.hpp"
#include "audio/audio_defs.hpp"
#include "audio/audio_engine.hpp"
#include "audio/audio_playback.hpp"
#include "audio/audio_stream.hpp"

#include "core/array_view.hpp"
#include "core/async.hpp"
#include "core/buffered_vector.hpp"
#include "core/callback_stream.hpp"
#include "core/cmd_line.hpp"
#include "core/crc32.hpp"
#include "core/data.hpp"
#include "core/date_time.hpp"
#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "core/ioc_container.hpp"
#include "core/log.hpp"
#include "core/mask.hpp"
#include "core/pool_vector.hpp"
#include "core/random.hpp"
#include "core/ref.hpp"
#include "core/sha256.hpp"
#include "core/signal.hpp"
#include "core/simple_id.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/string_utf.hpp"
#include "core/string_utils.hpp"
#include "core/synchronization.hpp"
#include "core/task.hpp"
#include "core/task_hnd.hpp"
#include "core/task_manager.hpp"
#include "core/task_parallel_for.hpp"
#include "core/timer.hpp"
#include "core/typed_array.hpp"
#include "core/typed_map.hpp"
#include "core/uuid.hpp"
#include "core/var.hpp"
#include "core/weak_ref.hpp"

#include "ecs/ecs_component.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_memory.hpp"
#include "ecs/ecs_query.hpp"
#include "ecs/ecs_registry.hpp"
#include "ecs/ecs_world.hpp"

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/gfx_resource.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_texture.hpp"
#include "gfx/gfx_vert_format.hpp"

#include "glsl/glsl_builder.hpp"
#include "glsl/glsl_include_processor.hpp"
#include "glsl/glsl_shader_compiler.hpp"

#include "grc/font.hpp"
#include "grc/icon.hpp"
#include "grc/image.hpp"
#include "grc/pso_cache.hpp"
#include "grc/pso_file.hpp"
#include "grc/shader.hpp"
#include "grc/shader_builder.hpp"
#include "grc/shader_cache.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_file.hpp"
#include "grc/shader_library.hpp"
#include "grc/shader_manager.hpp"
#include "grc/shader_param.hpp"
#include "grc/shader_param_block.hpp"
#include "grc/shader_reflection.hpp"
#include "grc/texture.hpp"
#include "grc/texture_compression.hpp"
#include "grc/texture_manager.hpp"
#include "grc/texture_resize.hpp"

#include "io/async_file_system.hpp"
#include "io/base64.hpp"
#include "io/compression.hpp"
#include "io/config.hpp"
#include "io/context.hpp"
#include "io/enum.hpp"
#include "io/ini.hpp"
#include "io/serialization.hpp"
#include "io/stream.hpp"
#include "io/stream_file.hpp"
#include "io/tree.hpp"

#include "material/material.hpp"

#include "math/color.hpp"
#include "math/frustum.hpp"
#include "math/mat.hpp"
#include "math/math_utils.hpp"
#include "math/math_utils2d.hpp"
#include "math/math_utils3d.hpp"
#include "math/plane.hpp"
#include "math/quat.hpp"
#include "math/range.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"

#include "memory/mem_linear.hpp"
#include "memory/mem_pool.hpp"

#include "mesh/mesh.hpp"
#include "mesh/mesh_array.hpp"
#include "mesh/mesh_batch.hpp"
#include "mesh/mesh_bucket.hpp"
#include "mesh/mesh_builder.hpp"
#include "mesh/mesh_manager.hpp"
#include "mesh/mesh_pass.hpp"

#include "pfx/pfx_component.hpp"
#include "pfx/pfx_effect.hpp"
#include "pfx/pfx_emitter.hpp"
#include "pfx/pfx_feature.hpp"

#include "platform/dll_manager.hpp"
#include "platform/file.hpp"
#include "platform/file_system.hpp"
#include "platform/input.hpp"
#include "platform/input_defs.hpp"
#include "platform/input_devices.hpp"
#include "platform/mount_volume.hpp"
#include "platform/time.hpp"
#include "platform/window.hpp"
#include "platform/window_manager.hpp"

#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"

#include "rdg/rdg_graph.hpp"
#include "rdg/rdg_pass.hpp"
#include "rdg/rdg_pool.hpp"
#include "rdg/rdg_resources.hpp"

#include "rtti/builtin.hpp"
#include "rtti/class.hpp"
#include "rtti/enum.hpp"
#include "rtti/function.hpp"
#include "rtti/meta_data.hpp"
#include "rtti/object.hpp"
#include "rtti/struct.hpp"
#include "rtti/traits.hpp"
#include "rtti/type.hpp"
#include "rtti/type_ref.hpp"
#include "rtti/type_storage.hpp"

#include "render/aux_draw_manager.hpp"
#include "render/camera.hpp"
#include "render/canvas.hpp"
#include "render/common.hpp"
#include "render/culling.hpp"
#include "render/graphics_pipeline.hpp"
#include "render/light.hpp"
#include "render/model.hpp"
#include "render/render_engine.hpp"
#include "render/render_queue.hpp"
#include "render/render_scene.hpp"
#include "render/view.hpp"
#include "render/view_manager.hpp"

#include "scene/scene.hpp"
#include "scene/scene_data.hpp"
#include "scene/scene_feature.hpp"
#include "scene/scene_manager.hpp"
#include "scene/scene_prefab.hpp"

#include "system/app.hpp"
#include "system/engine.hpp"
#include "system/engine_app.hpp"
#include "system/engine_config.hpp"
#include "system/engine_signals.hpp"
#include "system/game_app.hpp"
#include "system/game_plugin.hpp"
#include "system/hooks.hpp"
#include "system/plugin.hpp"
#include "system/plugin_manager.hpp"

#include "ui/ui_bars.hpp"
#include "ui/ui_binder.hpp"
#include "ui/ui_containers.hpp"
#include "ui/ui_content.hpp"
#include "ui/ui_controls.hpp"
#include "ui/ui_defs.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_inputs.hpp"
#include "ui/ui_manager.hpp"
#include "ui/ui_markup.hpp"
#include "ui/ui_query.hpp"
#include "ui/ui_style.hpp"
#include "ui/ui_tooltip.hpp"
#include "ui/ui_views.hpp"
#include "ui/ui_window.hpp"

#include "game/game_manager.hpp"