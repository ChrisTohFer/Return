#pragma once

#include <filesystem>
#include <optional>
#include <vector>

namespace re
{
    struct FileDialogContext
    {
        std::filesystem::path root_path;
        std::string extension;
    };
    struct FileDialogResult
    {
        std::filesystem::path result_path;
    };

    void save_file_dialog(const FileDialogContext&);
    void open_file_dialog(const FileDialogContext&);
    std::optional<FileDialogResult> update_file_dialog();
    bool file_dialog_open();
}