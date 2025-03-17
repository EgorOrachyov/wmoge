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
#include "asset/asset_ref.hpp"
#include "core/flat_map.hpp"
#include "grc/font.hpp"
#include "math/color.hpp"
#include "math/vec.hpp"
#include "rtti/traits.hpp"
#include "ui/ui_defs.hpp"

#include <optional>
#include <vector>

namespace wmoge {

    /** @brief Ui color palette */
    struct UiColorPalette {
        WG_RTTI_STRUCT(UiColorPalette);

        flat_map<Strid, Color4f> colors;
    };

    WG_RTTI_STRUCT_BEGIN(UiColorPalette) {
        WG_RTTI_FIELD(colors, {RttiOptional});
    }
    WG_RTTI_END;

    /** @brief Ui param slot override value */
    struct UiParamSlot {
        WG_RTTI_STRUCT(UiParamSlot);

        UiParam type;
        float   value;
    };

    WG_RTTI_STRUCT_BEGIN(UiParamSlot) {
        WG_RTTI_FIELD(type, {});
        WG_RTTI_FIELD(value, {});
    }
    WG_RTTI_END;

    /** @brief Ui color slot referencing named entry in palette */
    struct UiColorSlot {
        WG_RTTI_STRUCT(UiColorSlot);

        UiColor type;
        Strid   value;
    };

    WG_RTTI_STRUCT_BEGIN(UiColorSlot) {
        WG_RTTI_FIELD(type, {});
        WG_RTTI_FIELD(value, {});
    }
    WG_RTTI_END;

    /** @brief Ui font with tag to reference in elements styling */
    struct UiFontSlot {
        WG_RTTI_STRUCT(UiFontSlot);

        Strid          tag;
        AssetRef<Font> file;
    };

    WG_RTTI_STRUCT_BEGIN(UiFontSlot) {
        WG_RTTI_FIELD(tag, {});
        WG_RTTI_FIELD(file, {});
    }
    WG_RTTI_END;

    /** @brief Ui sub style which can be used to customize a sub tree of ui elements */
    struct UiSubStyle {
        WG_RTTI_STRUCT(UiSubStyle);

        Strid                    font;
        std::vector<UiParamSlot> params;
        std::vector<UiColorSlot> colors;
    };

    WG_RTTI_STRUCT_BEGIN(UiSubStyle) {
        WG_RTTI_FIELD(font, {RttiOptional});
        WG_RTTI_FIELD(params, {RttiOptional});
        WG_RTTI_FIELD(colors, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class UiStyleDesc
     * @brief Style desc which can be used for save/load of style
     */
    struct UiStyleDesc {
        WG_RTTI_STRUCT(UiStyleDesc);

        std::optional<float>        alpha;
        std::optional<float>        disabled_alpha;
        std::optional<Vec2f>        window_padding;
        std::optional<float>        window_rounding;
        std::optional<float>        window_border_size;
        std::optional<Vec2f>        window_min_size;
        std::optional<Vec2f>        window_title_align;
        std::optional<UiDir>        window_menu_button_position;
        std::optional<float>        child_rounding;
        std::optional<float>        child_border_size;
        std::optional<float>        popup_rounding;
        std::optional<float>        popup_border_size;
        std::optional<Vec2f>        frame_padding;
        std::optional<float>        frame_rounding;
        std::optional<float>        frame_border_size;
        std::optional<Vec2f>        item_spacing;
        std::optional<Vec2f>        item_inner_spacing;
        std::optional<Vec2f>        cell_padding;
        std::optional<Vec2f>        touch_extra_padding;
        std::optional<float>        indent_spacing;
        std::optional<float>        columns_min_spacing;
        std::optional<float>        scrollbar_size;
        std::optional<float>        scrollbar_rounding;
        std::optional<float>        grab_min_size;
        std::optional<float>        grab_rounding;
        std::optional<float>        log_slider_deadzone;
        std::optional<float>        tab_rounding;
        std::optional<float>        tab_border_size;
        std::optional<float>        tab_min_width_for_close_button;
        std::optional<float>        tab_bar_border_size;
        std::optional<float>        tab_bar_overline_size;
        std::optional<float>        table_angled_headers_angle;
        std::optional<Vec2f>        table_angled_headers_textAlign;
        std::optional<UiDir>        color_button_position;
        std::optional<Vec2f>        button_text_align;
        std::optional<Vec2f>        selectable_text_align;
        std::optional<float>        separator_text_border_size;
        std::optional<Vec2f>        separator_text_align;
        std::optional<Vec2f>        separator_text_padding;
        std::optional<Vec2f>        display_window_padding;
        std::optional<Vec2f>        display_safe_area_padding;
        std::optional<float>        docking_separator_size;
        std::optional<float>        mouse_cursor_scale;
        std::optional<bool>         anti_aliased_lines;
        std::optional<bool>         anti_aliased_lines_use_tex;
        std::optional<bool>         anti_aliased_fill;
        std::optional<float>        curve_tessellation_tol;
        std::optional<float>        circle_tessellation_max_error;
        std::optional<float>        hover_stationary_delay;
        std::optional<float>        hover_delay_short;
        std::optional<float>        hover_delay_normal;
        std::optional<float>        font_scale;
        std::vector<UiFontSlot>     fonts;
        std::vector<UiColorSlot>    colors;
        UiColorPalette              palette;
        flat_map<Strid, UiSubStyle> sub_styles;
    };

    WG_RTTI_STRUCT_BEGIN(UiStyleDesc) {
        WG_RTTI_FIELD(alpha, {RttiOptional});
        WG_RTTI_FIELD(disabled_alpha, {RttiOptional});
        WG_RTTI_FIELD(window_padding, {RttiOptional});
        WG_RTTI_FIELD(window_rounding, {RttiOptional});
        WG_RTTI_FIELD(window_border_size, {RttiOptional});
        WG_RTTI_FIELD(window_min_size, {RttiOptional});
        WG_RTTI_FIELD(window_title_align, {RttiOptional});
        WG_RTTI_FIELD(window_menu_button_position, {RttiOptional});
        WG_RTTI_FIELD(child_rounding, {RttiOptional});
        WG_RTTI_FIELD(child_border_size, {RttiOptional});
        WG_RTTI_FIELD(popup_rounding, {RttiOptional});
        WG_RTTI_FIELD(popup_border_size, {RttiOptional});
        WG_RTTI_FIELD(frame_padding, {RttiOptional});
        WG_RTTI_FIELD(frame_rounding, {RttiOptional});
        WG_RTTI_FIELD(frame_border_size, {RttiOptional});
        WG_RTTI_FIELD(item_spacing, {RttiOptional});
        WG_RTTI_FIELD(item_inner_spacing, {RttiOptional});
        WG_RTTI_FIELD(cell_padding, {RttiOptional});
        WG_RTTI_FIELD(touch_extra_padding, {RttiOptional});
        WG_RTTI_FIELD(indent_spacing, {RttiOptional});
        WG_RTTI_FIELD(columns_min_spacing, {RttiOptional});
        WG_RTTI_FIELD(scrollbar_size, {RttiOptional});
        WG_RTTI_FIELD(scrollbar_rounding, {RttiOptional});
        WG_RTTI_FIELD(grab_min_size, {RttiOptional});
        WG_RTTI_FIELD(grab_rounding, {RttiOptional});
        WG_RTTI_FIELD(log_slider_deadzone, {RttiOptional});
        WG_RTTI_FIELD(tab_rounding, {RttiOptional});
        WG_RTTI_FIELD(tab_border_size, {RttiOptional});
        WG_RTTI_FIELD(tab_min_width_for_close_button, {RttiOptional});
        WG_RTTI_FIELD(tab_bar_border_size, {RttiOptional});
        WG_RTTI_FIELD(tab_bar_overline_size, {RttiOptional});
        WG_RTTI_FIELD(table_angled_headers_angle, {RttiOptional});
        WG_RTTI_FIELD(table_angled_headers_textAlign, {RttiOptional});
        WG_RTTI_FIELD(color_button_position, {RttiOptional});
        WG_RTTI_FIELD(button_text_align, {RttiOptional});
        WG_RTTI_FIELD(selectable_text_align, {RttiOptional});
        WG_RTTI_FIELD(separator_text_border_size, {RttiOptional});
        WG_RTTI_FIELD(separator_text_align, {RttiOptional});
        WG_RTTI_FIELD(separator_text_padding, {RttiOptional});
        WG_RTTI_FIELD(display_window_padding, {RttiOptional});
        WG_RTTI_FIELD(display_safe_area_padding, {RttiOptional});
        WG_RTTI_FIELD(docking_separator_size, {RttiOptional});
        WG_RTTI_FIELD(mouse_cursor_scale, {RttiOptional});
        WG_RTTI_FIELD(anti_aliased_lines, {RttiOptional});
        WG_RTTI_FIELD(anti_aliased_lines_use_tex, {RttiOptional});
        WG_RTTI_FIELD(anti_aliased_fill, {RttiOptional});
        WG_RTTI_FIELD(curve_tessellation_tol, {RttiOptional});
        WG_RTTI_FIELD(circle_tessellation_max_error, {RttiOptional});
        WG_RTTI_FIELD(hover_stationary_delay, {RttiOptional});
        WG_RTTI_FIELD(hover_delay_short, {RttiOptional});
        WG_RTTI_FIELD(hover_delay_normal, {RttiOptional});
        WG_RTTI_FIELD(font_scale, {RttiOptional});
        WG_RTTI_FIELD(fonts, {RttiOptional});
        WG_RTTI_FIELD(colors, {RttiOptional});
        WG_RTTI_FIELD(palette, {RttiOptional});
        WG_RTTI_FIELD(sub_styles, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class UiStyle
     * @brief Storage for parameters for ui elements display
     */
    class UiStyle : public Asset {
    public:
        WG_RTTI_CLASS(UiStyle, Asset);

        void set_desc(UiStyleDesc desc) { m_desc = std::move(desc); }

        [[nodiscard]] const UiStyleDesc& get_desc() const { return m_desc; }

    private:
        UiStyleDesc m_desc;
    };

    WG_RTTI_CLASS_BEGIN(UiStyle) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_desc, {RttiOptional});
    }
    WG_RTTI_END;

}// namespace wmoge