#include "graphics_components.h"

namespace gfx
{
    VertexBuffer::VertexBuffer(const void* data, int vertex_count, const std::vector<VertexComponent>& components)
        : m_vertex_count(vertex_count)
        , m_components(components)
    {
        glGenBuffers(1, &m_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferData(GL_ARRAY_BUFFER, vertex_size(components.data(), components.size()) * vertex_count, data, GL_STATIC_DRAW);
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
        auto stride = vertex_size(m_components.data(), m_components.size());
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

    template<GLuint shader_type>
    Shader<shader_type>::Shader(const char* source, std::string* error_log)
    {
        m_id = glCreateShader(shader_type);
        glShaderSource(m_id, 1, &source, nullptr);
        glCompileShader(m_id);
        
        if(error_log)
        {
            //check for and report errors
            int success = false;
            glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                char buf[1024];
                glGetShaderInfoLog(m_id, sizeof(buf), nullptr, buf);
                *error_log = buf;
            }
        }
    }
    template<GLuint shader_type>
    Shader<shader_type>::Shader(Shader&& other)
        : m_id(other.m_id)
    {
        other.m_id = 0;
    }
    template<GLuint shader_type>
    Shader<shader_type>::~Shader()
    {
        if(m_id != 0)
        {
            glDeleteShader(m_id);
        }
    }
    template class Shader<GL_VERTEX_SHADER>;
    template class Shader<GL_FRAGMENT_SHADER>;
    
    
    ShaderProgram::ShaderProgram(const VertexShader& vshader, const FragmentShader& fshader, std::string *error_log)
    {
        m_id = glCreateProgram();
        glAttachShader(m_id, vshader.id());
        glAttachShader(m_id, fshader.id());
        glLinkProgram(m_id);

        if(error_log)
        {
            //check for and report errors
            int success = 0;
            glGetProgramiv(m_id, GL_LINK_STATUS, &success);
            if (!success)
            {
                char buf[1024];
                glGetProgramInfoLog(m_id, sizeof(buf), nullptr, buf);
                *error_log = buf;
            }
        }
    }
    ShaderProgram::ShaderProgram(ShaderProgram&& other)
        : m_id(other.m_id)
    {
        other.m_id = 0;
    }
    ShaderProgram::~ShaderProgram()
    {
        if(m_id != 0)
        {
            glDeleteProgram(m_id);
        }
    }
    void ShaderProgram::use() const
    {
        glUseProgram(m_id);
    }

    VertexArray::VertexArray(const VertexBuffer& vb, const ShaderProgram& program)
        : m_vb(&vb)
    {
        glGenVertexArrays(1, &m_id);
        glBindVertexArray(m_id);
        glBindBuffer(GL_ARRAY_BUFFER, vb.id());
        vb.bind_attributes();
        program.use();
    }
    VertexArray::VertexArray(VertexArray&& other)
        : m_id(other.m_id)
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
        glBindVertexArray(m_id);
        glDrawArrays(GL_TRIANGLES, 0, m_vb->vertex_count());
    }
}