#include "main_loop.h"

#include "graphics_test.h"
#include "window.h"

#include "gfx/graphics_manager.h"
#include "scene.h"

#include <chrono>
#include <iostream>

namespace re
{
    int main_loop(const EngineConfig& config)
    {
        std::cout << "Running " << CONFIGURATION_STR << " build.\n";

        //initiailize window
        if (!window_init(config.window_name, config.init_window_width, config.init_window_height))
        {
            std::cout << "Failed to create window.\n";
            return -1;
        }

        gfx::BatchRenderer batch_renderer;
        re::GraphicsTestEditor editor;
        re::Scene scene(batch_renderer, window_input_manager());
        
        auto time = std::chrono::system_clock::now();
        while (window_update())
        {
            //calculate dt
            float frame_time = 0.f;
            {
                auto new_time = std::chrono::system_clock::now();
                frame_time = 1e-9f * std::chrono::duration_cast<std::chrono::nanoseconds>(new_time - time).count();
                time = new_time;
            }

            //update scene
            scene.try_update();
            scene.draw(frame_time, window_aspect());
        }

        //shutdown window
        window_shutdown();

        return 0;
    }
}