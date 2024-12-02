#pragma once

#include "vertex_components.h"
#include "glad/glad.h"

#include <string>
#include <vector>

namespace gfx
{
    class VertexBuffer
    {
    public:
        VertexBuffer(const void* data, int vertex_count, const std::vector<VertexComponent>& components);
        VertexBuffer(VertexBuffer&&);
        ~VertexBuffer();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        int vertex_count() const { return m_vertex_count; }
        void bind_attributes() const;
    private:
        GLuint m_id = 0;
        int m_vertex_count = 0;
        std::vector<VertexComponent> m_components;
    };

    template<GLuint shader_type>
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

    using VertexShader   = Shader<GL_VERTEX_SHADER>;
    using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

    class ShaderProgram
    {
    public:
        ShaderProgram(const VertexShader&, const FragmentShader&, std::string* error_log = nullptr);
        ShaderProgram(ShaderProgram&&);
        ~ShaderProgram();
        
        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        void use() const;

    private:
        GLuint m_id;
    };

    class VertexArray
    {
    public:
        VertexArray(const VertexBuffer&, const ShaderProgram&);
        VertexArray(VertexArray&&);
        ~VertexArray();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        void draw_triangles() const;

    private:
        GLuint m_id;
        const VertexBuffer* m_vb;
    };
}