#pragma once

#include "imgui/imgui.h"

#include <string>
#include <type_traits>
#include <vector>

//imgui helper functions
//convention is to return true on edit and deactivate, not per keystroke

namespace imhelp//ing
{
    void display_error_if_present(const char* error);
    bool edit_multiline_string(const char* label, std::string& s);

    bool edit(const char* label, int& s);
    bool edit(const char* label, float& s);
    bool edit(const char* label, bool& s);
    bool edit(const char* label, std::string& s);

    template<typename ElementT, typename ...ContextArgs>
    bool edit_list(const char* label, std::vector<ElementT>& list, ContextArgs&&... args);

    //inline implementations ========================================================

    template<typename ElementT, typename ...ContextArgs>
    bool edit_list(const char* label, std::vector<ElementT>& list, ContextArgs&&... args)
    {
        bool changed = false;

        ImGui::PushID(label);
        ImGui::Text(label);
        ImGui::Indent();

        //iterate elements and give opportunity to edit each one
        int to_delete = -1;
        int to_move = -1;
        for (int i = 0; i < list.size(); ++i)
        {
            ImGui::PushID(i);
            auto& element = list[i];

            //allow user to remove or move the element
            if (ImGui::Button("-"))
            {
                to_delete = i;
            }
            ImGui::SameLine();
            if (ImGui::Button("^"))
            {
                to_move = i - 1;
            }
            ImGui::SameLine();
            if (ImGui::Button("v"))
            {
                to_move = i;
            }
            ImGui::SameLine();

            //allow user to edit the element
            if (edit("", element, args...)) changed = true;

            ImGui::PopID();
        }
        ImGui::Unindent();

        //allow adding new elements
        if (ImGui::Button("Add"))
        {
            list.push_back({});
            changed = true;
        }

        ImGui::PopID();

        //handle delete request
        if (to_delete != -1)
        {
            list.erase(list.begin() + to_delete);
            changed = true;
        }

        //handle move request
        if (to_move >= 0 && to_move < (int)list.size() - 1)
        {
            std::swap(list[to_move], list[to_move + 1]);
            changed = true;
        }

        return changed;
    }

}