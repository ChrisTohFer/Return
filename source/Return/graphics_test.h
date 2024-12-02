#pragma once

#include "graphics_components.h" 

#include <string>
#include <vector>


class VertexBuffer
{
public:
    struct Triangle
    {
        int a = 0;
        int b = 0;
        int c = 0;
    };

    static VertexBuffer create_triangle_buffer();

    bool edit();

    const std::string& name() const { return m_name; }
    std::string& error_log() { return m_error_log; }
    int total_size() const { return vertex_size() * m_num_vertices; }
    const void* data() const { return m_data.data(); }
    const std::vector<gfx::VertexComponent>& components() const { return m_components; }
    int vertex_size() const;
    int num_vertices() const { return m_num_vertices; }

private:
    bool edit_vertex(int i);
    std::string m_name;

    std::vector<gfx::VertexComponent> m_components;
    std::vector<uint8_t> m_data;
    int m_num_vertices = 0;
    std::vector<Triangle> m_triangles;

    std::string m_error_log;
};

template<unsigned shader_type>
class Shader
{
public:
    static Shader create_triangle_shader() requires (shader_type == GL_VERTEX_SHADER);
    static Shader create_triangle_shader() requires (shader_type == GL_FRAGMENT_SHADER);

    bool edit();

    const std::string& name() const { return m_name; }
    const std::string& source() const { return m_source; }
    std::string& error_log() { return m_error_log; }

private:
    std::string m_name;
    std::string m_source;
    std::vector<std::string> m_uniforms;

    std::string m_error_log;
};
using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

class ShaderProgram
{
public:
    static ShaderProgram create_default_triangle_program();
    bool edit();

    const std::string& name() const { return m_name; }
    const std::string& vertex_shader() const { return m_vert_shader_name; }
    const std::string& fragment_shader() const { return m_frag_shader_name; }
    std::string& error_log() { return m_error_log; }

private:
    std::string m_name;
    std::string m_vert_shader_name;
    std::string m_frag_shader_name;
    std::string m_error_log;
};

class VertexArrayObject
{
public:
    static VertexArrayObject create_default_triangle_vao();
    bool edit();

    const std::string& name() const { return m_name; }
    std::string& error_log() { return m_error_log; }
    const std::string& buffer_name() const { return m_vertex_buffer_name; }
    const std::string& program_name() const { return m_shader_program_name; }

private:
    std::string m_name;
    std::string m_vertex_buffer_name;
    std::string m_shader_program_name;
    std::string m_error_log;
};

class GraphicsTestEditor
{
public:
    struct Data
    {
        std::vector<VertexBuffer> m_vertex_buffers;
        std::vector<VertexShader> m_vertex_shaders;
        std::vector<FragmentShader> m_fragment_shaders;
        std::vector<ShaderProgram> m_shader_programs;
        std::vector<VertexArrayObject> m_vertex_array_objects;
    };

    GraphicsTestEditor();

    bool edit();

    Data& data() { return m_data; }

private:
    void snapshot();
    bool undo();
    bool redo();

    bool m_deferred_update = true;
    Data m_data;

    constexpr static int undo_stack_size = 32;
    Data m_undo_stack[undo_stack_size];
    int m_undo_length = 0;
    int m_redo_length = 0;
    int m_undo_current = 0;
};

class GraphicsTestPreview
{
public:
    void initialize(GraphicsTestEditor&);

    void draw() const;

private:
    std::vector<gfx::VertexBuffer>   m_compiled_buffers;
    std::vector<gfx::VertexShader>   m_compiled_vertex_shaders;
    std::vector<gfx::FragmentShader> m_compiled_fragment_shaders;
    std::vector<gfx::ShaderProgram>  m_compiled_shader_programs;
    std::vector<gfx::VertexArray>    m_compiled_vaos;
};