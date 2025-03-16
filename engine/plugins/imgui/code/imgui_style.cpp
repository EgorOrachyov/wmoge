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

#include "imgui_style.hpp"

namespace wmoge {

#define IMGUI_SET_VAL(imgui_field, ui_field)     \
    if (desc.ui_field.has_value()) {             \
        dst.imgui_field = desc.ui_field.value(); \
    }

#define IMGUI_SET_VAL_VEC2(imgui_field, ui_field)            \
    if (desc.ui_field.has_value()) {                         \
        dst.imgui_field = imgui_vec2(desc.ui_field.value()); \
    }

#define IMGUI_SET_VAL_DIR(imgui_field, ui_field)            \
    if (desc.ui_field.has_value()) {                        \
        dst.imgui_field = imgui_dir(desc.ui_field.value()); \
    }

#define IMGUI_GET_VAL(imgui_field, ui_field) \
    desc.ui_field = src.imgui_field;

#define IMGUI_GET_VAL_VEC2(imgui_field, ui_field) \
    desc.ui_field = imgui_vec2(src.imgui_field);

#define IMGUI_GET_VAL_DIR(imgui_field, ui_field) \
    desc.ui_field = imgui_dir(src.imgui_field);

    void imgui_style_to_imgui_style(const Ref<UiStyle>& src, ImGuiStyle& dst) {
        const UiStyleDesc&              desc    = src->get_desc();
        const UiColorPalette&           palette = desc.palette;
        const std::vector<UiColorSlot>& colors  = desc.colors;

        IMGUI_SET_VAL(Alpha, alpha);
        IMGUI_SET_VAL(DisabledAlpha, disabled_alpha);
        IMGUI_SET_VAL_VEC2(WindowPadding, window_padding);
        IMGUI_SET_VAL(WindowRounding, window_rounding);
        IMGUI_SET_VAL(WindowBorderSize, window_border_size);
        IMGUI_SET_VAL_VEC2(WindowMinSize, window_min_size);
        IMGUI_SET_VAL_VEC2(WindowTitleAlign, window_title_align);
        IMGUI_SET_VAL_DIR(WindowMenuButtonPosition, window_menu_button_position);
        IMGUI_SET_VAL(ChildRounding, child_rounding);
        IMGUI_SET_VAL(ChildBorderSize, child_border_size);
        IMGUI_SET_VAL(PopupRounding, popup_rounding);
        IMGUI_SET_VAL(PopupBorderSize, popup_border_size);
        IMGUI_SET_VAL_VEC2(FramePadding, frame_padding);
        IMGUI_SET_VAL(FrameRounding, frame_rounding);
        IMGUI_SET_VAL(FrameBorderSize, frame_border_size);
        IMGUI_SET_VAL_VEC2(ItemSpacing, item_spacing);
        IMGUI_SET_VAL_VEC2(ItemInnerSpacing, item_inner_spacing);
        IMGUI_SET_VAL_VEC2(CellPadding, cell_padding);
        IMGUI_SET_VAL_VEC2(TouchExtraPadding, touch_extra_padding);
        IMGUI_SET_VAL(IndentSpacing, indent_spacing);
        IMGUI_SET_VAL(ColumnsMinSpacing, columns_min_spacing);
        IMGUI_SET_VAL(ScrollbarSize, scrollbar_size);
        IMGUI_SET_VAL(ScrollbarRounding, scrollbar_rounding);
        IMGUI_SET_VAL(GrabMinSize, grab_min_size);
        IMGUI_SET_VAL(GrabRounding, grab_rounding);
        IMGUI_SET_VAL(LogSliderDeadzone, log_slider_deadzone);
        IMGUI_SET_VAL(TabRounding, tab_rounding);
        IMGUI_SET_VAL(TabBorderSize, tab_border_size);
        IMGUI_SET_VAL(TabMinWidthForCloseButton, tab_min_width_for_close_button);
        IMGUI_SET_VAL(TabBarBorderSize, tab_bar_border_size);
        IMGUI_SET_VAL(TabBarOverlineSize, tab_bar_overline_size);
        IMGUI_SET_VAL(TableAngledHeadersAngle, table_angled_headers_angle);
        IMGUI_SET_VAL_VEC2(TableAngledHeadersTextAlign, table_angled_headers_textAlign);
        IMGUI_SET_VAL_DIR(ColorButtonPosition, color_button_position);
        IMGUI_SET_VAL_VEC2(ButtonTextAlign, button_text_align);
        IMGUI_SET_VAL_VEC2(SelectableTextAlign, selectable_text_align);
        IMGUI_SET_VAL(SeparatorTextBorderSize, separator_text_border_size);
        IMGUI_SET_VAL_VEC2(SeparatorTextAlign, separator_text_align);
        IMGUI_SET_VAL_VEC2(SeparatorTextPadding, separator_text_padding);
        IMGUI_SET_VAL_VEC2(DisplayWindowPadding, display_window_padding);
        IMGUI_SET_VAL_VEC2(DisplaySafeAreaPadding, display_safe_area_padding);
        IMGUI_SET_VAL(DockingSeparatorSize, docking_separator_size);
        IMGUI_SET_VAL(MouseCursorScale, mouse_cursor_scale);
        IMGUI_SET_VAL(AntiAliasedLines, anti_aliased_lines);
        IMGUI_SET_VAL(AntiAliasedLinesUseTex, anti_aliased_lines_use_tex);
        IMGUI_SET_VAL(AntiAliasedFill, anti_aliased_fill);
        IMGUI_SET_VAL(CurveTessellationTol, curve_tessellation_tol);
        IMGUI_SET_VAL(CircleTessellationMaxError, circle_tessellation_max_error);
        IMGUI_SET_VAL(HoverStationaryDelay, hover_stationary_delay);
        IMGUI_SET_VAL(HoverDelayShort, hover_delay_short);
        IMGUI_SET_VAL(HoverDelayNormal, hover_delay_normal);

        for (const UiColorSlot& slot : colors) {
            auto color = palette.colors.find(slot.value);
            if (color == palette.colors.end()) {
                WG_LOG_ERROR("no such color in palette " << slot.value);
                continue;
            }
            dst.Colors[static_cast<ImGuiCol>(slot.type)] = imgui_color4(color->second);
        }
    }

    void imgui_style_from_imgui_style(const Ref<UiStyle>& dst, const ImGuiStyle& src) {
        UiStyleDesc               desc;
        UiColorPalette&           palette = desc.palette;
        std::vector<UiColorSlot>& colors  = desc.colors;

        IMGUI_GET_VAL(Alpha, alpha);
        IMGUI_GET_VAL(DisabledAlpha, disabled_alpha);
        IMGUI_GET_VAL_VEC2(WindowPadding, window_padding);
        IMGUI_GET_VAL(WindowRounding, window_rounding);
        IMGUI_GET_VAL(WindowBorderSize, window_border_size);
        IMGUI_GET_VAL_VEC2(WindowMinSize, window_min_size);
        IMGUI_GET_VAL_VEC2(WindowTitleAlign, window_title_align);
        IMGUI_GET_VAL_DIR(WindowMenuButtonPosition, window_menu_button_position);
        IMGUI_GET_VAL(ChildRounding, child_rounding);
        IMGUI_GET_VAL(ChildBorderSize, child_border_size);
        IMGUI_GET_VAL(PopupRounding, popup_rounding);
        IMGUI_GET_VAL(PopupBorderSize, popup_border_size);
        IMGUI_GET_VAL_VEC2(FramePadding, frame_padding);
        IMGUI_GET_VAL(FrameRounding, frame_rounding);
        IMGUI_GET_VAL(FrameBorderSize, frame_border_size);
        IMGUI_GET_VAL_VEC2(ItemSpacing, item_spacing);
        IMGUI_GET_VAL_VEC2(ItemInnerSpacing, item_inner_spacing);
        IMGUI_GET_VAL_VEC2(CellPadding, cell_padding);
        IMGUI_GET_VAL_VEC2(TouchExtraPadding, touch_extra_padding);
        IMGUI_GET_VAL(IndentSpacing, indent_spacing);
        IMGUI_GET_VAL(ColumnsMinSpacing, columns_min_spacing);
        IMGUI_GET_VAL(ScrollbarSize, scrollbar_size);
        IMGUI_GET_VAL(ScrollbarRounding, scrollbar_rounding);
        IMGUI_GET_VAL(GrabMinSize, grab_min_size);
        IMGUI_GET_VAL(GrabRounding, grab_rounding);
        IMGUI_GET_VAL(LogSliderDeadzone, log_slider_deadzone);
        IMGUI_GET_VAL(TabRounding, tab_rounding);
        IMGUI_GET_VAL(TabBorderSize, tab_border_size);
        IMGUI_GET_VAL(TabMinWidthForCloseButton, tab_min_width_for_close_button);
        IMGUI_GET_VAL(TabBarBorderSize, tab_bar_border_size);
        IMGUI_GET_VAL(TabBarOverlineSize, tab_bar_overline_size);
        IMGUI_GET_VAL(TableAngledHeadersAngle, table_angled_headers_angle);
        IMGUI_GET_VAL_VEC2(TableAngledHeadersTextAlign, table_angled_headers_textAlign);
        IMGUI_GET_VAL_DIR(ColorButtonPosition, color_button_position);
        IMGUI_GET_VAL_VEC2(ButtonTextAlign, button_text_align);
        IMGUI_GET_VAL_VEC2(SelectableTextAlign, selectable_text_align);
        IMGUI_GET_VAL(SeparatorTextBorderSize, separator_text_border_size);
        IMGUI_GET_VAL_VEC2(SeparatorTextAlign, separator_text_align);
        IMGUI_GET_VAL_VEC2(SeparatorTextPadding, separator_text_padding);
        IMGUI_GET_VAL_VEC2(DisplayWindowPadding, display_window_padding);
        IMGUI_GET_VAL_VEC2(DisplaySafeAreaPadding, display_safe_area_padding);
        IMGUI_GET_VAL(DockingSeparatorSize, docking_separator_size);
        IMGUI_GET_VAL(MouseCursorScale, mouse_cursor_scale);
        IMGUI_GET_VAL(AntiAliasedLines, anti_aliased_lines);
        IMGUI_GET_VAL(AntiAliasedLinesUseTex, anti_aliased_lines_use_tex);
        IMGUI_GET_VAL(AntiAliasedFill, anti_aliased_fill);
        IMGUI_GET_VAL(CurveTessellationTol, curve_tessellation_tol);
        IMGUI_GET_VAL(CircleTessellationMaxError, circle_tessellation_max_error);
        IMGUI_GET_VAL(HoverStationaryDelay, hover_stationary_delay);
        IMGUI_GET_VAL(HoverDelayShort, hover_delay_short);
        IMGUI_GET_VAL(HoverDelayNormal, hover_delay_normal);

        std::vector<std::pair<Strid, Color4f>> color_cache;
        int                                    color_cache_size = 0;

        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            const UiColor color_type  = static_cast<UiColor>(i);
            const Color4f color_value = imgui_color4(src.Colors[i]);

            std::optional<Strid> color_name;

            for (const auto& cached_color : color_cache) {
                if (cached_color.second == color_value) {
                    color_name = cached_color.first;
                    break;
                }
            }

            if (!color_name) {
                color_name = SID("color_" + std::to_string(color_cache_size));
                color_cache_size++;
                color_cache.emplace_back(*color_name, color_value);
            }

            UiColorSlot& slot = colors.emplace_back();
            slot.type         = color_type;
            slot.value        = *color_name;
        }

        for (const auto& cached_color : color_cache) {
            palette.colors[cached_color.first] = cached_color.second;
        }

        dst->set_desc(std::move(desc));
    }

}// namespace wmoge