#include "imgui_helpers.h"

#include "maths/vector2.h"
#include "maths/vector3.h"

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

    bool edit(const char* label, geom::Vector2& v)
    {
        float float2[2] = { v.x, v.y };
        ImGui::InputFloat2(label, float2);
        v = { float2[0],float2[1] };
        return ImGui::IsItemDeactivatedAfterEdit();;
    }

    bool edit(const char* label, geom::Vector3& v)
    {
        float float3[3] = { v.x, v.y, v.z };
        ImGui::InputFloat3(label, float3);
        v = { float3[0],float3[1],float3[2] };
        return ImGui::IsItemDeactivatedAfterEdit();;
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
    Indent::Indent()
    {
        ImGui::Indent();
    }
    Indent::~Indent()
    {
        ImGui::Unindent();
    }
}