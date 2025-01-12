#pragma once

#include "imgui/imgui.h"

namespace re
{
    void begin_dockspace()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f,0.f));

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::Begin("DockspaceHolder", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBringToFrontOnFocus);
            
        ImGui::PopStyleVar(3);

        ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0.f,0.f),
            ImGuiDockNodeFlags_NoDockingInCentralNode |
            ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void end_dockspace()
    {
        ImGui::End();
    }
}