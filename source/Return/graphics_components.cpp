#include "graphics_components.h"

#include <assert.h>
#include <cstring>
#include <iostream>

namespace gfx
{
    static const char* get_gl_error_string()
    {
        auto error = glGetError();
        switch (error)
        {
        case GL_INVALID_ENUM:       return "GL_INVALID_ENUM";      break;
        case GL_INVALID_VALUE:      return "GL_INVALID_VALUE";     break;
        case GL_INVALID_OPERATION:  return "GL_INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:     return "GL_STACK_OVERFLOW";    break;
        case GL_STACK_UNDERFLOW:    return "GL_STACK_UNDERFLOW";   break;
        case GL_OUT_OF_MEMORY:      return "GL_OUT_OF_MEMORY";     break;
        case GL_NO_ERROR:           return "";     break;
        default:                    assert(!"Unrecognised error type"); return "";
        }
    }
    void report_gl_error()
    {
    #ifdef DEBUG
        auto error_string = get_gl_error_string();
        if (strcmp(error_string, ""))
        {
            std::cout << error_string << "\n";
        }
    #endif
    }

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

    int ShaderProgram::uniform_location(const char *name) const
    {
        return glGetUniformLocation(m_id, name);
    }

    VertexArray::VertexArray(const VertexBuffer& vb, const ShaderProgram& program, const ElementBuffer* eb)
        : m_vb(&vb)
        , m_sp(&program)
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
        program.use();
    }
    VertexArray::VertexArray(VertexArray&& other)
        : m_id(other.m_id)
        , m_vb(other.m_vb)
        , m_sp(other.m_sp)
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

    int VertexArray::uniform_location(const char *name) const
    {
        return m_sp ? m_sp->uniform_location(name) : -1;
    }

    void set_uniform(GLint location, float value)                 { glUniform1f(location, value); }
    void set_uniform(GLint location, bool value)                  { glUniform1i(location, value); }
    void set_uniform(GLint location, int value)                   { glUniform1i(location, value); }
    void set_uniform(GLint location, geom::Vector2 value)         { glUniform2f(location, value.x, value.y); }
    void set_uniform(GLint location, const geom::Vector3& value)  { glUniform3f(location, value.x, value.y, value.z); }
    void set_uniform(GLint location, const geom::Matrix44& value) { glUniformMatrix4fv(location, 1, false, value.values); }
}