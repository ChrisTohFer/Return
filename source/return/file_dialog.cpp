#include "file_dialog.h"

#include "fuzzy_search.h"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include <algorithm>

namespace re
{
    enum class FileDialogMode
    {
        Open,
        Save
    };
    struct SearchResult
    {
        std::filesystem::path path;
        float levenshtein_distance;
    };

    static FileDialogMode g_mode;

    static std::string g_search_string;
    static bool g_recursive_search;
    static std::vector<SearchResult> g_search_results;

    static std::filesystem::path g_current_path;
    static std::string g_file_name;
    static FileDialogContext g_context;
    

    bool current_path_is_sub_path_of_root(std::filesystem::path root_path)
    {
        auto relative = std::filesystem::relative(g_current_path, root_path);
        return !relative.empty() && relative.native()[0] != '.';
    }

    //open functions
    static void file_dialog(const FileDialogContext& context)
    {
        g_context = context;
        g_search_string = "";
        g_search_results.clear();
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
    static void apply_search_to_path(const std::filesystem::path& path)
    {
        bool is_directory = std::filesystem::is_directory(path);
        if (!is_directory && !g_context.extension.empty() && path.extension() != g_context.extension)
        {
            return;
        }
        g_search_results.push_back({
            path,
            levenshtein_distance(path.filename().c_str(), g_search_string.c_str())
            });
    }
    static void apply_search()
    {
        g_search_results.clear();
        if(g_recursive_search)
        {
            for(auto entry : std::filesystem::recursive_directory_iterator(g_current_path))
            {
                apply_search_to_path(entry.path());
            }
        }
        else
        {
            for(auto entry : std::filesystem::directory_iterator(g_current_path))
            {
                apply_search_to_path(entry.path());
            }
        }

        std::sort(g_search_results.begin(), g_search_results.end(), [](auto& lhs, auto& rhs){ return lhs.levenshtein_distance < rhs.levenshtein_distance; });
    }

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
                apply_search();
            }

            //list files/folders in current directory OR search results
            if(ImGui::BeginListBox("files"))
            {
                auto display_entry = [&](const std::filesystem::path& entry, bool is_directory)
                {
                    char buf[512];
                    snprintf(buf, sizeof(buf), "%s%s", is_directory ? "F> " : "   ", entry.filename().c_str());
                    ImGui::Selectable(buf);
                    if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
                    {
                        if(is_directory)
                        {
                            g_current_path = entry;
                        }
                        else
                        {
                            ImGui::CloseCurrentPopup();
                            result = {entry};
                        }
                    }
                };
                if(g_search_string.empty())
                {
                    for(auto entry : std::filesystem::directory_iterator(g_current_path))
                    {
                        bool is_directory = entry.is_directory();
                        if(!is_directory && !g_context.extension.empty() && entry.path().extension() != g_context.extension)
                        {
                            continue;
                        }

                        display_entry(entry.path(), is_directory);
                    }
                }
                else
                {
                    for(auto result : g_search_results)
                    {
                        display_entry(result.path, std::filesystem::is_directory(result.path));
                        ImGui::SameLine();
                        ImGui::Text("%f", result.levenshtein_distance);
                    }
                }
                ImGui::EndListBox();
            }
            
            //resolve and display full filename
            ImGui::InputText("Filename", &g_file_name);
            auto full_filename = g_current_path;
            full_filename.append(g_file_name);
            if(!g_context.extension.empty() && full_filename.extension() != g_context.extension)
            {
                full_filename.concat(g_context.extension);
            }
            ImGui::TextUnformatted(full_filename.c_str());

            //open/save
            const char* button_name = "";
            switch(g_mode)
            {
            case FileDialogMode::Open: button_name = "Open"; break;
            case FileDialogMode::Save: button_name = "Save"; break;
            }
            ImGui::BeginDisabled(
                g_file_name.empty() ||
                (g_mode == FileDialogMode::Open && !std::filesystem::exists(full_filename)));
            if(ImGui::Button(button_name))
            {
                ImGui::CloseCurrentPopup();
                result = {full_filename};
            }
            ImGui::EndDisabled();

            //cancel
            ImGui::SameLine();
            if(ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return result;
    }
    
    bool file_dialog_open()
    {
        return ImGui::IsPopupOpen("File");
    }
}