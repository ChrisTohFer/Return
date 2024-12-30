#include "vertex_array_object.h"

#include "vertex_buffer.h"
#include "element_buffer.h"

#include "glad/glad.h"

namespace gfx
{
    VertexArray::VertexArray(const VertexBuffer& vb, const ElementBuffer* eb)
        : m_vb(&vb)
        , m_eb(eb)
    {
        glGenVertexArrays(1, &m_id);
        glBindVertexArray(m_id);
        glBindBuffer(GL_ARRAY_BUFFER, vb.id());
        m_vb->bind_attributes();
        if (m_eb)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb->id());
        }
    }
    VertexArray::VertexArray(VertexArray&& other)
        : m_id(other.m_id)
        , m_vb(other.m_vb)
        , m_eb(other.m_eb)
    {
        other.m_id = 0;
    }
    VertexArray::~VertexArray()
    {
        if(m_id != 0)
        {
            glDeleteVertexArrays(1, &m_id);
        }
    }
    void VertexArray::draw_triangles() const
    {
        if(m_id != 0 && m_vb != nullptr)
        {
            glBindVertexArray(m_id);
            if (m_eb)
            {
                glDrawElements(GL_TRIANGLES, m_eb->triangle_count() * 3, GL_UNSIGNED_INT, nullptr);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, m_vb->vertex_count());
            }
            glBindVertexArray(0);
        }
    }
    void VertexArray::draw_lines() const
    {
        if(m_id != 0 && m_vb != nullptr)
        {
            glBindVertexArray(m_id);
            glDrawArrays(GL_LINES, 0, m_vb->vertex_count());
            glBindVertexArray(0);
        }
    }
}