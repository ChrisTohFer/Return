#include "graphics_core.h"

#include "glad/glad.h"
#include <iostream>

namespace gfx
{
    bool init(void(*(*proc_address)(const char*))(), int width, int height)
    {
        if (!gladLoadGLLoader((GLADloadproc)proc_address))
        {
            std::cout << "gladLoadGLLoader failed" << std::endl;
            return false;
        }

        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        return true;
    }

    void shutdown()
    {
    }

    void clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void resize_viewport(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}