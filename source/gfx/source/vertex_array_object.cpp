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
    void VertexArray::draw_triangles(const VertexBuffer* instance_buffer) const
    {
        if(m_id != 0 && m_vb != nullptr)
        {
            if(instance_buffer)
            {
                instance_buffer->bind_attributes();
                if (m_eb)
                {
                    glDrawElementsInstanced(GL_TRIANGLES, m_eb->triangle_count() * 3, GL_UNSIGNED_INT, nullptr, instance_buffer->vertex_count());
                }
                else
                {
                    glDrawArraysInstanced(GL_TRIANGLES, 0, m_vb->vertex_count(), instance_buffer->vertex_count());
                }
            }
            else
            {
                if (m_eb)
                {
                    glDrawElements(GL_TRIANGLES, m_eb->triangle_count() * 3, GL_UNSIGNED_INT, nullptr);
                }
                else
                {
                    glDrawArrays(GL_TRIANGLES, 0, m_vb->vertex_count());
                }
            }
        }
    }
    void VertexArray::draw_lines(const VertexBuffer* instance_buffer) const
    {
        if(m_id != 0 && m_vb != nullptr)
        {
            if(instance_buffer)
            {
                instance_buffer->bind_attributes();
                glDrawArraysInstanced(GL_LINES, 0, m_vb->vertex_count(), instance_buffer->vertex_count());
            }
            else
            {
                glDrawArrays(GL_LINES, 0, m_vb->vertex_count());
            }
            glBindVertexArray(m_id);
            glDrawArrays(GL_LINES, 0, m_vb->vertex_count());
            glBindVertexArray(0);
        }
    }
    void VertexArray::use() const
    {
        glBindVertexArray(m_id);
    }
}