#pragma once

#include "gfx/graphics_manager.h"
#include "file/file.h"

#include <string>
#include <vector>

namespace gfx
{
    DEFINE_ENUM_SERIALIZE_FUNCTIONS(gfx::BufferAttributeType)
}

namespace re
{
    class VertexBuffer
    {
    public:
        static VertexBuffer create_triangle_buffer();

        bool edit();

        const std::string& name() const { return m_name; }
        std::string& error_log() { return m_error_log; }
        int total_size() const { return vertex_size() * m_num_vertices; }
        const void* data() const { return m_data.data(); }
        const std::vector<gfx::BufferAttributeType>& components() const { return m_components; }
        int vertex_size() const;
        int num_vertices() const { return m_num_vertices; }

        DEFINE_SERIALIZATION_FUNCTIONS(m_name, m_components, m_data);

    private:
        bool edit_vertex(int i);

        std::string m_name;
        std::vector<gfx::BufferAttributeType> m_components;
        std::vector<uint8_t> m_data;
        int m_num_vertices = 0;

        std::string m_error_log;
    };

    class ElementBuffer
    {
    public:
        struct Triangle
        {
            unsigned a, b, c;
            DEFINE_SERIALIZATION_FUNCTIONS(a,b,c);
        };
        bool edit();

        const std::string& name() const { return m_name; }
        std::string& error_log() { return m_error_log; }
        const void* data() const { return m_triangles.data(); }
        int num_triangles() const { return (int)m_triangles.size(); }
        
        DEFINE_SERIALIZATION_FUNCTIONS(m_name, m_triangles);

    private:
        std::string m_name;
        std::vector<Triangle> m_triangles;

        std::string m_error_log;
    };

    class VertexArrayObject
    {
    public:
        static VertexArrayObject create_default_triangle_vao();
        bool edit();

        const std::string& name() const { return m_name; }
        std::string& error_log() { return m_error_log; }
        const std::string& vertex_buffer_name() const { return m_vertex_buffer_name; }
        const std::string& element_buffer_name() const { return m_element_buffer_name; }
        
        DEFINE_SERIALIZATION_FUNCTIONS(m_name, m_vertex_buffer_name,m_element_buffer_name)

    private:
        std::string m_name;
        std::string m_vertex_buffer_name;
        std::string m_element_buffer_name;
        std::string m_error_log;
    };

    template<gfx::ShaderType shader_type>
    class Shader
    {
    public:
        static Shader create_triangle_shader() requires (shader_type == gfx::ShaderType::Vertex);
        static Shader create_triangle_shader() requires (shader_type == gfx::ShaderType::Fragment);

        bool edit();

        const std::string& name() const { return m_name; }
        const std::string& source() const { return m_source; }
        std::string& error_log() { return m_error_log; }
        
        DEFINE_SERIALIZATION_FUNCTIONS(m_name, m_source)

    private:
        std::string m_name;
        std::string m_source;

        std::string m_error_log;
    };
    using VertexShader = Shader<gfx::ShaderType::Vertex>;
    using FragmentShader = Shader<gfx::ShaderType::Fragment>;

    class ShaderProgram
    {
    public:
        static ShaderProgram create_default_triangle_program();
        bool edit();

        const std::string& name() const { return m_name; }
        const std::string& vertex_shader() const { return m_vert_shader_name; }
        const std::string& fragment_shader() const { return m_frag_shader_name; }
        std::string& error_log() { return m_error_log; }
        
        DEFINE_SERIALIZATION_FUNCTIONS(m_name, m_vert_shader_name, m_frag_shader_name)

    private:
        std::string m_name;
        std::string m_vert_shader_name;
        std::string m_frag_shader_name;
        std::string m_error_log;
    };

    class Texture
    {
    public:
        static Texture default_wall_texture();

        bool edit();

        const std::string& name() const { return m_name; }
        const std::string& texture_filename() const { return m_filename; }
        std::string& error_log() { return m_error_log; }

        DEFINE_SERIALIZATION_FUNCTIONS(m_name, m_filename)
        
    private:
        std::string m_name;
        std::string m_filename;
        std::string m_error_log;
    };

    class GraphicsTestEditor
    {
    public:
        struct Data
        {
            std::vector<VertexBuffer> m_vertex_buffers;
            std::vector<ElementBuffer> m_element_buffers;
            std::vector<VertexShader> m_vertex_shaders;
            std::vector<FragmentShader> m_fragment_shaders;
            std::vector<ShaderProgram> m_shader_programs;
            std::vector<VertexArrayObject> m_vertex_array_objects;
            std::vector<Texture> m_textures;

            DEFINE_SERIALIZATION_FUNCTIONS(m_vertex_buffers, m_element_buffers, m_vertex_shaders, m_fragment_shaders, m_shader_programs, m_vertex_array_objects, m_textures)
        };

        GraphicsTestEditor();

        bool edit();

        Data& data() { return m_data; }

        void compile_assets(gfx::GraphicsManager& manager);

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
}