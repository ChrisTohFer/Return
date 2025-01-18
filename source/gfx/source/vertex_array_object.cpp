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

    void VertexArray::draw() const
    {
        if(m_id == 0 || m_vb == nullptr)
        {
            return;
        }

        glBindVertexArray(m_id);
        int gl_primitive_type = GL_TRIANGLES;
        switch(m_vb->get_primitive_type())
        {
        case PrimitiveType::Line:     gl_primitive_type = GL_LINES;     break;
        case PrimitiveType::Triangle: gl_primitive_type = GL_TRIANGLES; break;
        }

        if (m_eb)
        {
            glDrawElements(gl_primitive_type, m_eb->element_count(), GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawArrays(gl_primitive_type, 0, m_vb->vertex_count());
        }
        glBindVertexArray(0);
    }

    void VertexArray::draw(const VertexBuffer& instance_buffer) const
    {
        if(m_id == 0 || m_vb == nullptr)
        {
            return;
        }

        glBindVertexArray(m_id);
        int gl_primitive_type = GL_TRIANGLES;
        switch(m_vb->get_primitive_type())
        {
        case PrimitiveType::Line:     gl_primitive_type = GL_LINES;     break;
        case PrimitiveType::Triangle: gl_primitive_type = GL_TRIANGLES; break;
        }

        instance_buffer.bind_attributes();
        if (m_eb)
        {
            glDrawElementsInstanced(gl_primitive_type, m_eb->element_count(), GL_UNSIGNED_INT, nullptr, instance_buffer.vertex_count());
        }
        else
        {
            glDrawArraysInstanced(gl_primitive_type, 0, m_vb->vertex_count(), instance_buffer.vertex_count());
        }
        glBindVertexArray(0);
    }

}