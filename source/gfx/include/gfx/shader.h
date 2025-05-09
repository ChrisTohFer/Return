#pragma once

#include "gfx_forward.h"

#include "maths/maths.h"
#include "maths/vector2.h"

#include <string>

namespace gfx
{
    enum class ShaderType
    {
        Vertex,
        Fragment
    };

    template<ShaderType shader_type>
    class Shader
    {
    public:
        Shader(const char* source, std::string* error_log = nullptr);
        Shader(Shader&&);
        ~Shader();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }

    private:
        GLuint m_id = 0;
    };

    using VertexShader   = Shader<ShaderType::Vertex>;
    using FragmentShader = Shader<ShaderType::Fragment>;

    class ShaderProgram
    {
    public:
        ShaderProgram() = default;
        ShaderProgram(const VertexShader&, const FragmentShader&, std::string* error_log = nullptr);
        ShaderProgram(ShaderProgram&&);
        ~ShaderProgram();
        
        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        void use() const;
        int uniform_location(const char* name) const;

    private:
        GLuint m_id = 0;
    };

    void set_uniform(GLint location, float);
    void set_uniform(GLint location, bool);
    void set_uniform(GLint location, int);
    void set_uniform(GLint location, const maths::Vector2&);
    void set_uniform(GLint location, const maths::Vector3&);
    void set_uniform(GLint location, const maths::Matrix44&);
}