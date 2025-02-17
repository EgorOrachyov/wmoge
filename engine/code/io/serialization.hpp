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

#include "core/log.hpp"
#include "core/mask.hpp"
#include "core/status.hpp"
#include "io/stream.hpp"
#include "io/tree.hpp"

#include <type_traits>

namespace wmoge {

    enum class IoFlag {
        ReadOptional = 0
    };

    using IoFlags = Mask<IoFlag>;

    struct IoTagRead;
    struct IoTagWrite;

#define WG_IO_DECLARE(cls)                                                        \
    friend Status tree_read(IoContext& context, IoTree& tree, cls& value);        \
    friend Status tree_write(IoContext& context, IoTree& tree, const cls& value); \
    friend Status stream_read(IoContext& context, IoStream& stream, cls& value);  \
    friend Status stream_write(IoContext& context, IoStream& stream, const cls& value);

#define WG_IO_IMPLEMENT(nmsp, trg, cls)                                                                     \
    Status tree_read(IoContext& context, IoTree& tree, trg& value) {                                        \
        return nmsp##__##cls##Serializer<const YamlConstNodeRef&, trg&, IoTagRead>()(context, tree, value); \
    }                                                                                                       \
    Status tree_write(IoContext& context, IoTree& tree, const trg& value) {                                 \
        return nmsp##__##cls##Serializer<YamlNodeRef, const trg&, IoTagWrite>()(context, tree, value);      \
    }                                                                                                       \
    Status stream_read(IoContext& context, IoStream& stream, trg& value) {                                  \
        return nmsp##__##cls##Serializer<IoStream&, trg&, IoTagRead>()(context, stream, value);             \
    }                                                                                                       \
    Status stream_write(IoContext& context, IoStream& stream, const trg& value) {                           \
        return nmsp##__##cls##Serializer<IoStream&, const trg&, IoTagWrite>()(context, stream, value);      \
    }

#define WG_IO_SUPER(super)                                                                \
    if constexpr (std::is_same_v<Stream, const YamlConstNodeRef&>) {                      \
        WG_TREE_READ_SUPER(context, stream, super, target);                               \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, YamlNodeRef>) {                                  \
        WG_TREE_WRITE_SUPER(context, stream, super, target);                              \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, IoStream&> && std::is_same_v<Tag, IoTagRead>) {  \
        WG_ARCHIVE_READ_SUPER(context, stream, super, target);                            \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, IoStream&> && std::is_same_v<Tag, IoTagWrite>) { \
        WG_ARCHIVE_WRITE_SUPER(context, stream, super, target);                           \
    }

#define WG_IO_BEGIN_NMSP(nmsp, cls)                                                \
    template<typename Stream, typename Target, typename Tag>                       \
    struct nmsp##__##cls##Serializer final {                                       \
        Status operator()(IoContext& context, Stream stream, Target target) {      \
            static const char* profile_mark_tree_read     = #cls "::tree_read";    \
            static const char* profile_mark_tree_write    = #cls "::tree_write";   \
            static const char* profile_mark_archive_read  = #cls "::stream_read";  \
            static const char* profile_mark_archive_write = #cls "::stream_write"; \
            if constexpr (std::is_same_v<Stream, YamlNodeRef>) {                   \
                WG_TREE_MAP(stream);                                               \
            }

#define WG_IO_BEGIN(cls) WG_IO_BEGIN_NMSP(global, cls)

#define WG_IO_PROFILE()                                                                   \
    const char* profile_mark_name = "";                                                   \
    if constexpr (std::is_same_v<Stream, const YamlConstNodeRef&>) {                      \
        profile_mark_name = profile_mark_tree_read;                                       \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, YamlNodeRef>) {                                  \
        profile_mark_name = profile_mark_tree_write;                                      \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, IoStream&> && std::is_same_v<Tag, IoTagRead>) {  \
        profile_mark_name = profile_mark_archive_read;                                    \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, IoStream&> && std::is_same_v<Tag, IoTagWrite>) { \
        profile_mark_name = profile_mark_archive_write;                                   \
    }                                                                                     \
    WG_PROFILE_CPU_IO(profile_mark_name);

#define WG_IO_FIELD_EXT(field, name, flags)                                               \
    if constexpr (std::is_same_v<Stream, const YamlConstNodeRef&>) {                      \
        const IoFlags mask = flags;                                                       \
        if (mask.get(IoFlag::ReadOptional)) {                                             \
            WG_TREE_READ_AS_OPT(context, stream, name, target.field);                     \
        } else {                                                                          \
            WG_TREE_READ_AS(context, stream, name, target.field);                         \
        }                                                                                 \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, YamlNodeRef>) {                                  \
        WG_TREE_WRITE_AS(context, stream, name, target.field);                            \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, IoStream&> && std::is_same_v<Tag, IoTagRead>) {  \
        WG_ARCHIVE_READ(context, stream, target.field);                                   \
    }                                                                                     \
    if constexpr (std::is_same_v<Stream, IoStream&> && std::is_same_v<Tag, IoTagWrite>) { \
        WG_ARCHIVE_WRITE(context, stream, target.field);                                  \
    }

#define WG_IO_FIELD_OPT(field) WG_IO_FIELD_EXT(field, #field, {IoFlag::ReadOptional})

#define WG_IO_FIELD(field) WG_IO_FIELD_EXT(field, #field, {})

#define WG_IO_FIELD_AS(field, name) WG_IO_FIELD_EXT(field, name, {})

#define WG_IO_END_NMSP(nmsp, cls)          \
    return WG_OK;                          \
    }                                      \
    ;                                      \
    }                                      \
    ;                                      \
    using nmsp##__##cls##Type = nmsp::cls; \
    WG_IO_IMPLEMENT(nmsp, nmsp##__##cls##Type, cls)

#define WG_IO_END(cls)               \
    return WG_OK;                    \
    }                                \
    ;                                \
    }                                \
    ;                                \
    using global__##cls##Type = cls; \
    WG_IO_IMPLEMENT(global, global__##cls##Type, cls)

}// namespace wmoge