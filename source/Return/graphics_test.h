#pragma once

#include "glad/glad.h"

#include <string>
#include <vector>


/*require classes to represent:
* -vertex buffer (vertex data, vertex format, id)
* -vertex shader (source, id)
* -fragment shader (source, id)
* -vertex array object (refs to other components, id)
* 
* also give each class a name, error log
*/

enum class ValueType
{
    Float,
    Int,
    Bool,
    Vec2,
    Vec3,
    Mat43
};

class VertexBuffer
{
public:
    struct Triangle
    {
        int a = 0;
        int b = 0;
        int c = 0;
    };

    bool edit();

    const std::string& name() const { return m_name; }
    std::string& error_log() { return m_error_log; }

private:
    bool edit_vertex(int i);
    int vertex_size() const;
    std::string m_name;

    std::vector<ValueType> m_components;
    std::vector<uint8_t> m_data;
    int m_num_vertices = 0;
    std::vector<Triangle> m_triangles;

    std::string m_error_log;
};

template<unsigned shader_type>
class Shader
{
public:
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
    bool edit();

    const std::string& name() const { return m_name; }
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
    bool edit();

    const std::string& name() const { return m_name; }
    std::string& error_log() { return m_error_log; }

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

    bool edit();

    Data& data() { return m_data; }

private:
    void snapshot();
    void undo();
    void redo();

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

private:
    std::vector<GLuint> m_buffer_ids;
    std::vector<GLuint> m_vertex_shader_ids;
    std::vector<GLuint> m_fragment_shader_ids;
    std::vector<GLuint> m_shader_program_ids;
    std::vector<GLuint> m_vao_ids;
};