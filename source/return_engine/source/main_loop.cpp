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

        gfx::GraphicsManager manager;
        re::GraphicsTestEditor editor;
        re::Scene scene(manager, window_input_manager());
        
        auto time = std::chrono::system_clock::now();
        while (window_update())
        {
            //calculate dt
            float dt = 0.f;
            {
                auto new_time = std::chrono::system_clock::now();
                dt = 1e-9f * std::chrono::duration_cast<std::chrono::nanoseconds>(new_time - time).count();
                time = new_time;
            }

            //update editor
            if(editor.edit())
            {
                editor.compile_assets(manager);
                scene.relink_assets();
            }
            scene.editor_ui();

            //update scene
            scene.update_and_draw(dt, window_aspect());
        }

        //shutdown window
        window_shutdown();

        return 0;
    }
}