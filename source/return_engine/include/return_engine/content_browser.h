#pragma once

#include <filesystem>
#include <vector>

/*UNFINISHED*/

namespace re
{

    //content browser is for showing all the files in the data folder
    //it should support viewing content of individual folders, fuzzy searching all files, and should only reload from disc when prompted
    //contextual options for each filetype should be provided, in particular the option to open the file in the editor

    class ContentBrowser
    {
    public:
        ContentBrowser() { refresh_cache(); }
        void draw();

    private:
        struct Entry
        {
            std::filesystem::path relative_path;
            std::vector<Entry> sub_entries;
        };

        void apply_filter(const Entry& entry);
        void apply_filter();
        void refresh_cache();

        std::vector<Entry> m_entries;
        const Entry* m_selected_entry = nullptr;
        const Entry* m_current_directory = nullptr;

        struct SearchResult
        {
            const Entry* entry = nullptr;
            float distance = 0.f;
        };
        std::string m_search_filter;
        std::vector<SearchResult> m_results;
    };
}