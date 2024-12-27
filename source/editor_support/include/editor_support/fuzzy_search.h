#pragma once

namespace re
{
    //substitution cost is relative to addition/subtraction cost of 1
    float levenshtein_distance(const char* s1, const char* s2, float substitution_cost = 1.f);
    
    //substitution cost is relative to addition/subtraction cost of 1
    float adjusted_levenshtein_distance(const char* text, const char* search_term, float substitution_cost = 1.f);
}