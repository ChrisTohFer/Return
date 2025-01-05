#include "vertex_buffer.h"

#include "glad/glad.h"

namespace gfx
{
    VertexBuffer::VertexBuffer(const void* data, int vertex_count, const std::vector<BufferAttributeType>& components)
        : m_vertex_count(vertex_count)
        , m_components(components)
    {
        glGenBuffers(1, &m_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferData(GL_ARRAY_BUFFER, vertex_size(components.data(), (int)components.size()) * vertex_count, data, GL_STATIC_DRAW);
    }
    VertexBuffer::VertexBuffer(VertexBuffer&& other)
        : m_id(other.m_id)
        , m_vertex_count(other.m_vertex_count)
        , m_components(std::move(other.m_components))
    {
        other.m_id = 0;
    }
    VertexBuffer::~VertexBuffer()
    {
        if(m_id != 0)
        {
            glDeleteBuffers(1, &m_id);
        }
    }
    void VertexBuffer::bind_attributes() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        
        auto stride = vertex_size(m_components.data(), (int)m_components.size());
        uint64_t offset = 0;
        for(auto& component : m_components)
        {
            glVertexAttribPointer((int)component, attribute_opengl_count(component), attribute_opengl_type(component), GL_FALSE, stride, (void*)offset);
            glEnableVertexAttribArray((int)component);
            glVertexAttribDivisor((int)component, attribute_instanced(component) ? 1 : 0);
            offset += attribute_size(component); 
        }
    }
}