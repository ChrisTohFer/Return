#pragma once

#include "vertex_buffer.h"
#include "element_buffer.h"
#include "vertex_array_object.h"
#include "shader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace gfx
{
    void report_gl_error();

    class GraphicsManager
    {
    public:
        void clear();

        void add(const char* name, std::unique_ptr<gfx::VertexBuffer>&& obj);
        void add(const char* name, std::unique_ptr<gfx::ElementBuffer>&& obj);
        void add(const char* name, std::unique_ptr<gfx::VertexArray>&& obj);
        void add(const char* name, std::unique_ptr<gfx::VertexShader>&& obj);
        void add(const char* name, std::unique_ptr<gfx::FragmentShader>&& obj);
        void add(const char* name, std::unique_ptr<gfx::ShaderProgram>&& obj);

        std::vector<std::string> vertex_buffer_names() const;
        std::vector<std::string> element_buffer_names() const;
        std::vector<std::string> vertex_array_names() const;
        std::vector<std::string> vertex_shader_names() const;
        std::vector<std::string> fragment_shader_names() const;
        std::vector<std::string> shader_program_names() const;

        const gfx::VertexBuffer*   vertex_buffer(const char* name) const;
        const gfx::ElementBuffer*  element_buffer(const char* name) const;
        const gfx::VertexArray*    vertex_array(const char* name) const;
        const gfx::VertexShader*   vertex_shader(const char* name) const;
        const gfx::FragmentShader* fragment_shader(const char* name) const;
        const gfx::ShaderProgram*  shader_program(const char* name) const;

    private:
        std::unordered_map<std::string, std::unique_ptr<gfx::VertexBuffer>>   m_vertex_buffers;
        std::unordered_map<std::string, std::unique_ptr<gfx::ElementBuffer>>  m_element_buffers;
        std::unordered_map<std::string, std::unique_ptr<gfx::VertexArray>>    m_vertex_arrays;
        std::unordered_map<std::string, std::unique_ptr<gfx::VertexShader>>   m_vertex_shaders;
        std::unordered_map<std::string, std::unique_ptr<gfx::FragmentShader>> m_fragment_shaders;
        std::unordered_map<std::string, std::unique_ptr<gfx::ShaderProgram>>  m_shader_programs;
    };
}