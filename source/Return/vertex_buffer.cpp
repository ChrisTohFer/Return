#include "vertex_buffer.h"

#include "glad/glad.h"

namespace gfx
{
    VertexBuffer::VertexBuffer(const void* data, int vertex_count, const std::vector<VertexComponent>& components)
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
        auto stride = vertex_size(m_components.data(), (int)m_components.size());
        uint64_t offset = 0;
        for(int i = 0; i < m_components.size(); ++i)
        {
            auto component = m_components[i];
            switch(component)
            {
            case VertexComponent::Float: glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, stride, (void*)offset); break;
            case VertexComponent::Int:   glVertexAttribPointer(i, 1, GL_INT,   GL_FALSE, stride, (void*)offset); break;
            case VertexComponent::Bool:  glVertexAttribPointer(i, 1, GL_BOOL,  GL_FALSE, stride, (void*)offset); break;
            case VertexComponent::Vec2:  glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset); break;
            case VertexComponent::Vec3:  glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset); break;
            default: break;
            }
            offset += component_size(component); 
            glEnableVertexAttribArray(i);
        }
    }
}