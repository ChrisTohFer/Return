#pragma once

#include "return_engine/input_manager.h"
#include "return_engine/scene.h"

#include "gfx/batch_renderer.h"

#include <functional>
#include <memory>

namespace re
{
    struct EngineConfig
    {
        const char* window_name        = "Return Engine";
        int         init_window_width  = 700;
        int         init_window_height = 700;
    };

    int main_loop(const EngineConfig&, std::function<std::unique_ptr<Scene>(gfx::BatchRenderer&, const InputManager&)>);
}