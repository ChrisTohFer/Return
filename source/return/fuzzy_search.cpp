#include "fuzzy_search.h"

#include <assert.h>
#include <cctype>
#include <cstring>
#include <vector>

namespace re
{
    float levenshtein_distance(const char* s1, const char* s2, float substitution_cost)
    {
        //see wikipedia page for explanation and original algorithm
        const auto l1 = strlen(s1);
        const auto columns = l1 + 1;
        const auto l2 = strlen(s2);
        const auto rows = l2 + 1;

        //create a matrix of distances
        std::vector<float> distances;
        distances.resize(columns*rows, 0.f);
        auto distance = [&distances, columns](int column, int row) -> float&
        {
            return distances[column + row * columns];
        };

        for(int column = 0; column < columns; ++column)
        {
            distance(column, 0) = (float)column;
        }
        for(int row = 0; row < rows; ++row)
        {
            distance(0, row) = (float)row;
        }
        
        for(int column = 1; column < columns; ++column)
        {
            for(int row = 1; row < rows; ++row)
            {
                const bool match = tolower(s1[column - 1]) == tolower(s2[row-1]);
                const float row_shift = distance(column, row-1) + 1;
                const float col_shift = distance(column-1, row) + 1;
                const float d_shift = distance(column-1, row-1) + (match ? 0.f : substitution_cost);
                distance(column, row) = 
                    std::min(std::min(row_shift, col_shift), d_shift);
            }
        }

        return distances.back();
    }

    float adjusted_levenshtein_distance(const char* text, const char* search_term, float substitution_cost)
    {
        if(strstr(text, search_term) != nullptr)
        {
            //if search term is a substring then use alternative scoring, based on how much of the text we matched
            auto text_length = strlen(text);
            auto search_length = strlen(search_term);
            assert(text_length != 0);
            return 1.f - (float)search_length / (float)text_length;
        }
        return levenshtein_distance(text, search_term, substitution_cost);
    }
}