#pragma once

namespace gfx
{
    //this is definitely NOT the best way to do this...
    bool init(void(*(*proc_address)(const char*))(), int width, int height);
    void shutdown();
    void clear(float r, float g, float b, float a);

    void resize_viewport(int width, int height);
}