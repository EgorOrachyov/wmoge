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

#include "audio_source_2d.hpp"

#include "audio/audio_bus.hpp"
#include "audio/audio_engine.hpp"
#include "core/engine.hpp"
#include "resource/audio_stream.hpp"
#include "resource/resource_manager.hpp"
#include "scene/scene_object.hpp"

namespace wmoge {

    void AudioSource2d::create(Ref<AudioStream> stream, StringId bus) {
        assert(stream);

        auto* engine       = Engine::instance();
        auto* audio_engine = engine->audio_engine();

        if (!stream) {
            WG_LOG_ERROR("passed null audio stream");
            return;
        }

        if (bus.empty()) {
            bus = audio_engine->get_default_bus();
        }

        m_playback = audio_engine->make_playback(stream, bus, get_scene_object()->get_name());

        if (!m_playback) {
            WG_LOG_ERROR("failed to make playback for stream " << stream->get_name());
            return;
        }

        m_playback->set_position(Vec3f(Math2d::transform(get_matr_global(), m_position), 0.0f));
        m_playback->set_velocity(Vec3f(Math2d::transform(get_matr_global(), m_velocity), 0.0f));
        m_playback->set_direction(Vec3f(Math2d::transform(get_matr_global(), m_direction), 0.0f));
        m_playback->set_pitch_scale(m_pitch_scale);
        m_playback->set_gain(m_gain);
        m_playback->set_min_gain(m_min_gain);
        m_playback->set_max_gain(m_max_gain);
        m_playback->set_max_distance(m_max_distance);
        m_playback->set_loop(m_loop);
    }
    void AudioSource2d::play() {
        assert(m_playback);
        if (m_playback) m_playback->play();
    }
    void AudioSource2d::stop() {
        assert(m_playback);
        if (m_playback) m_playback->stop();
    }
    bool AudioSource2d::has_playback() {
        return m_playback;
    }

    void AudioSource2d::set_position(Vec2f value) {
        m_position = value;

        assert(m_playback);
        if (m_playback) m_playback->set_position(Vec3f(Math2d::transform(get_matr_global(), value), 0.0f));
    }
    void AudioSource2d::set_velocity(Vec2f value) {
        m_velocity = value;

        assert(m_playback);
        if (m_playback) m_playback->set_velocity(Vec3f(Math2d::transform(get_matr_global(), value), 0.0f));
    }
    void AudioSource2d::set_direction(Vec2f value) {
        m_direction = value;

        assert(m_playback);
        if (m_playback) m_playback->set_direction(Vec3f(Math2d::transform(get_matr_global(), value), 0.0f));
    }
    void AudioSource2d::set_pitch_scale(float value) {
        m_pitch_scale = value;

        assert(m_playback);
        if (m_playback) m_playback->set_pitch_scale(value);
    }
    void AudioSource2d::set_gain(float value) {
        m_gain = value;

        assert(m_playback);
        if (m_playback) m_playback->set_gain(value);
    }
    void AudioSource2d::set_min_gain(float value) {
        m_min_gain = value;

        assert(m_playback);
        if (m_playback) m_playback->set_min_gain(value);
    }
    void AudioSource2d::set_max_gain(float value) {
        m_max_gain = value;

        assert(m_playback);
        if (m_playback) m_playback->set_max_gain(value);
    }
    void AudioSource2d::set_max_distance(float value) {
        m_max_distance = value;

        assert(m_playback);
        if (m_playback) m_playback->set_max_distance(value);
    }

    Vec2f AudioSource2d::get_position() const {
        return m_position;
    }
    Vec2f AudioSource2d::get_velocity() const {
        return m_velocity;
    }
    Vec2f AudioSource2d::get_direction() const {
        return m_direction;
    }
    float AudioSource2d::get_pitch_scale() const {
        return m_pitch_scale;
    }
    float AudioSource2d::get_gain() const {
        return m_gain;
    }
    float AudioSource2d::get_min_gain() const {
        return m_min_gain;
    }
    float AudioSource2d::get_max_gain() const {
        return m_max_gain;
    }
    float AudioSource2d::get_max_distance() const {
        return m_max_distance;
    }

    bool AudioSource2d::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!CanvasItem::on_load_from_yaml(node)) {
            return false;
        }

        auto* engine           = Engine::instance();
        auto* resource_manager = engine->resource_manager();
        auto* audio_engine     = engine->audio_engine();
        auto  stream_name      = Yaml::read_sid(node["stream"]);
        auto  stream           = resource_manager->load(stream_name).cast<AudioStream>();

        if (!stream) {
            WG_LOG_ERROR("failed to load audio stream " << stream_name);
            return false;
        }

        auto bus = audio_engine->get_default_bus();
        if (node.has_child("bus")) {
            bus = Yaml::read_sid(node["bus"]);
        }
        if (node.has_child("pitch_scale")) {
            node["pitch_scale"] >> m_pitch_scale;
        }
        if (node.has_child("gain")) {
            node["gain"] >> m_gain;
        }
        if (node.has_child("min_gain")) {
            node["min_gain"] >> m_min_gain;
        }
        if (node.has_child("max_gain")) {
            node["max_gain"] >> m_max_gain;
        }
        if (node.has_child("loop")) {
            node["loop"] >> m_loop;
        }
        if (node.has_child("autoplay")) {
            node["autoplay"] >> m_autoplay;
        }

        create(stream, bus);

        if (has_playback() && m_autoplay) {
            m_playback->play();
        }

        return true;
    }
    void AudioSource2d::on_transform_updated() {
        CanvasItem::on_transform_updated();

        if (has_playback()) {
            const Mat3x3f& mat = get_matr_global();

            const Vec2f world_position  = Math2d::transform(mat, m_position);
            const Vec2f world_direction = Math2d::transform(mat, m_direction);
            const Vec2f world_velocity  = Math2d::transform(mat, m_velocity);

            m_playback->set_position(Vec3f(world_position, 0.0f));
            m_playback->set_direction(Vec3f(world_direction, 0.0f));
            m_playback->set_velocity(Vec3f(world_velocity, 0.0f));
        }
    }
    void AudioSource2d::on_scene_enter() {
        CanvasItem::on_scene_enter();

        if (has_playback() && m_autoplay) {
            m_playback->play();
        }
    }

}// namespace wmoge
