#pragma once

#include "gfx_forward.h"

namespace gfx
{
    class ElementBuffer
    {
    public:
        ElementBuffer(const void* data, int triangle_count);
        ElementBuffer(ElementBuffer&&);
        ~ElementBuffer();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        int triangle_count() const { return m_triangle_count; }
    private:
        GLuint m_id = 0;
        int m_triangle_count = 0;
    };
}