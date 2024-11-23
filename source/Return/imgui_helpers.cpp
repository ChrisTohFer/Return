#include "imgui_helpers.h"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

namespace imhelp
{
    bool edit_multiline_string(const char* label, std::string& s)
    {
        ImGui::InputTextMultiline(label, &s);
        return ImGui::IsItemDeactivatedAfterEdit();
    }

    bool edit(const char* label, int& i)
    {
        ImGui::InputInt(label, &i);
        return ImGui::IsItemDeactivatedAfterEdit();
    }

    bool edit(const char* label, float& f)
    {
        ImGui::InputFloat(label, &f);
        return ImGui::IsItemDeactivatedAfterEdit();
    }

    bool edit(const char* label, bool& b)
    {
        return ImGui::Checkbox(label, &b);
    }

    bool edit(const char* label, std::string& s)
    {
        ImGui::InputText(label, &s);
        return ImGui::IsItemDeactivatedAfterEdit();
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