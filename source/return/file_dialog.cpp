#include "file_dialog.h"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

namespace re
{
    enum class FileDialogMode
    {
        Open,
        Save
    };
    static FileDialogMode g_mode;

    static std::string g_search_string;
    static bool g_recursive_search;

    static std::filesystem::path g_current_path;
    static std::string g_file_name;
    static FileDialogContext g_context;

    bool current_path_is_sub_path_of_root(std::filesystem::path root_path)
    {
        auto relative = std::filesystem::relative(g_current_path, root_path);
        return !relative.empty() && relative.native()[0] != '.';
    }

    //open functions
    void file_dialog(const FileDialogContext& context)
    {
        g_context = context;
        g_search_string = "";
        if(!current_path_is_sub_path_of_root(context.root_path))
        {
            g_current_path = context.root_path;
        }
        ImGui::OpenPopup("File");
    }
    void save_file_dialog(const FileDialogContext& context)
    {
        g_mode = FileDialogMode::Save;
        file_dialog(context);
    }
    void open_file_dialog(const FileDialogContext& context)
    {
        g_mode = FileDialogMode::Open;
        file_dialog(context);
    }

    //update function
    std::optional<FileDialogResult> update_file_dialog()
    {
        std::optional<FileDialogResult> result;

        if(ImGui::BeginPopupModal("File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            //display current path relative to root
            ImGui::Text("Root path: %s", g_context.root_path.c_str());
            ImGui::Text("Relative path: %s", std::filesystem::relative(g_current_path, g_context.root_path).c_str());
            
            //allow user to step up a directory
            ImGui::SameLine();
            ImGui::BeginDisabled(!current_path_is_sub_path_of_root(g_context.root_path));
            if(ImGui::Button("^"))
            {
                g_current_path = g_current_path.parent_path();
            }
            ImGui::EndDisabled();

            //search
            bool search_string_updated = ImGui::InputText("Search", &g_search_string);
            ImGui::SameLine();
            ImGui::Checkbox("Recursive", &g_recursive_search);
            if(search_string_updated)
            {
                //update search results
            }

            //list files/folders in current directory OR search results
            if(ImGui::BeginListBox("files"))
            {
                for(auto entry : std::filesystem::directory_iterator(g_current_path))
                {
                    bool is_directory = entry.is_directory();
                    if(!is_directory && !g_context.extension.empty() && entry.path().extension() != g_context.extension)
                    {
                        continue;
                    }

                    char buf[512];
                    snprintf(buf, sizeof(buf), "%s%s", is_directory ? "F> " : "   ", entry.path().filename().c_str());
                    ImGui::Selectable(buf);
                    if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
                    {
                        if(is_directory)
                        {
                            g_current_path = entry.path();
                        }
                        else
                        {
                            ImGui::CloseCurrentPopup();
                            result = {entry.path()};
                        }
                    }
                }
                ImGui::EndListBox();
            }

            ImGui::InputText("Filename", &g_file_name);

            //open/save
            const char* button_name = "";
            switch(g_mode)
            {
            case FileDialogMode::Open: button_name = "Open"; break;
            case FileDialogMode::Save: button_name = "Save"; break;
            }

            auto full_filename = g_current_path;
            full_filename.append(g_file_name);
            if(!g_context.extension.empty() && full_filename.extension() != g_context.extension)
            {
                full_filename.concat(g_context.extension);
            }
            ImGui::TextUnformatted(full_filename.c_str());
            ImGui::BeginDisabled(
                g_file_name.empty() ||
                (g_mode == FileDialogMode::Open && !std::filesystem::exists(full_filename)));
            if(ImGui::Button(button_name))
            {
                ImGui::CloseCurrentPopup();
                result = {full_filename};
            }
            ImGui::EndDisabled();

            ///cancel
            ImGui::SameLine();
            if(ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return result;
    }
}