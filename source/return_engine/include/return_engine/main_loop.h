#pragma once

namespace re
{
    struct EngineConfig
    {
        const char* window_name        = "Return Engine";
        int         init_window_width  = 700;
        int         init_window_height = 700;
    };

    int main_loop(const EngineConfig&);
}