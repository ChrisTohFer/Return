#pragma once

#include "gfx_forward.h"

namespace gfx
{
    class ElementBuffer
    {
    public:
        ElementBuffer(const void* data, int element_count);
        ElementBuffer(ElementBuffer&&);
        ~ElementBuffer();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        int element_count() const { return m_element_count; }
    private:
        GLuint m_id = 0;
        int m_element_count = 0;
    };
}