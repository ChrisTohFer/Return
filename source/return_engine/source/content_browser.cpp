#include "content_browser.h"

#include "editor_support/fuzzy_search.h"
#include "file/file.h"

#include "imgui/imgui.h"

#include <algorithm>

namespace re
{

    void ContentBrowser::draw()
    {
        //button to refresh
        //search filter
        //list of entries - either current folder or search results
        //current path relative to data folder
        //double click action
    }

    void ContentBrowser::apply_filter()
    {
        m_results.clear();
        if(m_search_filter.empty())
        {
            return;
        }

        for(auto& entry : m_entries)
        {
            apply_filter(entry);
        }

        std::sort(m_results.begin(), m_results.end(), [](auto& lhs, auto& rhs){ return lhs.distance < rhs.distance;});
    }

    void ContentBrowser::apply_filter(const Entry& entry)
    {
        m_results.push_back({&entry, adjusted_levenshtein_distance(entry.relative_path.string().c_str(), m_search_filter.c_str())});
        for(auto& sub_entry : entry.sub_entries)
        {
            apply_filter(sub_entry);
        }
    }

    void ContentBrowser::refresh_cache()
    {
        m_results.clear();
        m_entries.clear();

        for(auto iter : std::filesystem::directory_iterator(file::get_data_path("")))
        {
            //todo: push back entries with relative path, and if directory then recursively push sub entries
        }

        apply_filter();
    }

}