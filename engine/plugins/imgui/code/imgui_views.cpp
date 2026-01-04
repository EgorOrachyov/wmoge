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

#include "imgui_views.hpp"

#include "imgui_manager.hpp"

namespace wmoge {

    static ImGuiTableFlags imgui_get_table_flags(UiTableFlags flags) {
        ImGuiTableFlags table_flags = 0;

        if (flags.get(UiTableFlag::Resizable)) {
            table_flags |= ImGuiTableFlags_Resizable;
        }
        if (flags.get(UiTableFlag::RowBg)) {
            table_flags |= ImGuiTableFlags_RowBg;
        }
        if (flags.get(UiTableFlag::BordersInnerH)) {
            table_flags |= ImGuiTableFlags_BordersInnerH;
        }
        if (flags.get(UiTableFlag::BordersOuterH)) {
            table_flags |= ImGuiTableFlags_BordersOuterH;
        }
        if (flags.get(UiTableFlag::BordersInnerV)) {
            table_flags |= ImGuiTableFlags_BordersInnerV;
        }
        if (flags.get(UiTableFlag::BordersOuterV)) {
            table_flags |= ImGuiTableFlags_BordersOuterV;
        }
        if (flags.get(UiTableFlag::BordersH)) {
            table_flags |= ImGuiTableFlags_BordersH;
        }
        if (flags.get(UiTableFlag::BordersV)) {
            table_flags |= ImGuiTableFlags_BordersV;
        }
        if (flags.get(UiTableFlag::BordersInner)) {
            table_flags |= ImGuiTableFlags_Borders;
        }
        if (flags.get(UiTableFlag::BordersOuter)) {
            table_flags |= ImGuiTableFlags_Borders;
        }
        if (flags.get(UiTableFlag::Borders)) {
            table_flags |= ImGuiTableFlags_Borders;
        }
        if (flags.get(UiTableFlag::ScrollX)) {
            table_flags |= ImGuiTableFlags_ScrollX;
        }
        if (flags.get(UiTableFlag::ScrollY)) {
            table_flags |= ImGuiTableFlags_ScrollY;
        }

        return table_flags;
    }

    static ImGuiTableColumnFlags imgui_get_table_column_flags(UiTableColumnFlags flags) {
        ImGuiTableColumnFlags column_flags = 0;

        return column_flags;
    }

    static ImGuiTreeNodeFlags imgui_get_table_row_flags(UiTableTreeRowFlags flags) {
        ImGuiTreeNodeFlags row_flags = 0;

        if (flags.get(UiTableTreeRowFlag::Leaf)) {
            row_flags |= ImGuiTreeNodeFlags_Leaf;
        }
        if (flags.get(UiTableTreeRowFlag::Bullet)) {
            row_flags |= ImGuiTreeNodeFlags_Bullet;
        }
        if (flags.get(UiTableTreeRowFlag::LabelSpanAllColumns)) {
            row_flags |= ImGuiTreeNodeFlags_LabelSpanAllColumns;
        }

        return row_flags;
    }

    void imgui_process_table_tree_row(ImguiProcessor& processor, UiTableTree& table, int num_columns, const UiTableTreeRow& row) {

        int num_sub_rows = static_cast<int>(row.sub_rows.size());
        int num_elements = static_cast<int>(row.children.size());
        int num_min      = Math::min(num_columns, num_elements);

        ImGui::PushID(&row);

        bool is_open = false;

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGuiTreeNodeFlags row_flags = imgui_get_table_row_flags(row.flags);

        is_open = ImGui::TreeNodeEx("##", row_flags);
        ImGui::SameLine();

        processor.process(row.children[0].get());

        for (int i = 1; i < num_min; i++) {
            ImGui::TableNextColumn();
            processor.process(row.children[i].get());
        }
        for (int i = num_min; i < num_columns; i++) {
            ImGui::NextColumn();
        }

        if (is_open) {
            for (int i = 0; i < num_sub_rows; i++) {
                if (row.sub_rows[i]->type == UiElementType::TableTreeRow) {
                    imgui_process_table_tree_row(processor, table, num_columns, *((UiTableTreeRow*) row.sub_rows[i].get()));
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void imgui_process_table_tree(ImguiProcessor& processor, UiTableTree& table) {
        ImGuiTableFlags table_flags = imgui_get_table_flags(table.flags);

        const int num_columns = static_cast<int>(table.columns.size());
        const int num_rows    = static_cast<int>(table.rows.size());

        if (num_columns <= 0) {
            return;
        }

        if (ImGui::BeginTable(table.title.c_str(), num_columns, table_flags)) {
            for (int i = 0; i < num_columns; i++) {
                const UiTableColumn&  column       = *table.columns[i];
                ImGuiTableColumnFlags column_flags = imgui_get_table_column_flags(column.flags);
                ImGui::TableSetupColumn(column.title.c_str(), column_flags);
            }

            ImGui::TableHeadersRow();

            for (int i = 0; i < num_rows; i++) {
                const UiTableTreeRow& row = *table.rows[i];
                imgui_process_table_tree_row(processor, table, num_columns, row);
            }

            ImGui::EndTable();
        }
    }

}// namespace wmoge