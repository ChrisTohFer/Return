#include "shader.h"

#include "glad/glad.h"

namespace gfx
{
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

    void set_uniform(GLint location, float value)                 { glUniform1f(location, value); }
    void set_uniform(GLint location, bool value)                  { glUniform1i(location, value); }
    void set_uniform(GLint location, int value)                   { glUniform1i(location, value); }
    void set_uniform(GLint location, maths::Vector2 value)         { glUniform2f(location, value.x, value.y); }
    void set_uniform(GLint location, const maths::Vector3& value)  { glUniform3f(location, value.x, value.y, value.z); }
    void set_uniform(GLint location, const maths::Matrix44& value) { glUniformMatrix4fv(location, 1, false, value.values); }
}