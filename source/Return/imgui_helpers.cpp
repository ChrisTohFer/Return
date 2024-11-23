#include "imgui_helpers.h"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

namespace imhelp
{
    bool edit_string(const char* label, std::string& s)
    {
        bool changed = false;
        ImGui::InputText(label, &s);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            changed = true;
        }
        return changed;
    }

    bool edit_multiline_string(const char* label, std::string& s)
    {
        bool changed = false;
        ImGui::InputTextMultiline(label, &s);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            changed = true;
        }
        return changed;
    }

    void display_error_if_present(const char* error)
    {
        if (error && strlen(error) != 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
            ImGui::TextWrapped(error);
            ImGui::PopStyleColor();
        }
    }
}