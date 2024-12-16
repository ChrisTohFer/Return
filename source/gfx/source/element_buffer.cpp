#include "element_buffer.h"

#include "glad/glad.h"

namespace gfx
{
    ElementBuffer::ElementBuffer(const void* data, int triangle_count)
        : m_triangle_count(triangle_count)
    {
        glGenBuffers(1, &m_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_count * sizeof(int) * 3, data, GL_STATIC_DRAW);
    }
    ElementBuffer::ElementBuffer(ElementBuffer&& other)
        : m_id(other.m_id)
        , m_triangle_count(other.m_triangle_count)
    {
        other.m_id = 0;
    }
    ElementBuffer::~ElementBuffer()
    {
        if (m_id != 0)
        {
            glDeleteBuffers(1, &m_id);
        }
    }
}