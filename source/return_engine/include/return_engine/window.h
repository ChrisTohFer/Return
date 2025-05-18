#pragma once

#include "input_manager.h"

struct GLFWwindow;

namespace re
{
    //returns false if init failed for any reason
    bool window_init(const char* window_name, int width, int height);
    void window_shutdown();

    //return true if execution should continue, false if should exit
    bool window_update();

    float window_aspect();
    const InputManager& window_input_manager();
}