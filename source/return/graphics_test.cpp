#include "graphics_test.h"

#include "file_dialog.h"
#include "imgui_helpers.h"

#include "maths/vector2.h"
#include "maths/maths.h"

#include "gfx/image.h"

#include "glad/glad.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include <algorithm>
#include <format>

namespace gfx
{
    static bool edit(const char* label, gfx::VertexComponent& vt)
    {
        int index = (int)vt;
        bool changed = ImGui::Combo(label, &index, gfx::g_vertex_component_names, (int)gfx::VertexComponent::Num);
        vt = gfx::VertexComponent(index);
        return changed;
    }
}

namespace re
{
    //VertexBuffer ==================================================================

    VertexBuffer VertexBuffer::create_triangle_buffer()
    {
        VertexBuffer triangle_buffer;
        triangle_buffer.m_name = "triangle";
        triangle_buffer.m_components = { gfx::VertexComponent::Vec3, gfx::VertexComponent::Vec2 };
        triangle_buffer.m_num_vertices = 3;
        triangle_buffer.m_data.resize(3 * (sizeof(maths::Vector3) + sizeof(maths::Vector2)));

        auto* floats = reinterpret_cast<float*>(triangle_buffer.m_data.data());
        floats[0] = -0.5f; floats[1] = -0.5f; floats[2] = 0.f; floats[3] = -0.5f; floats[4] = -0.5f;
        floats[5] = 0.5f; floats[6] = -0.5f; floats[7] = 0.f; floats[8] = 0.5f; floats[9] = -0.5f;
        floats[10] = 0.f; floats[11] = 0.5f; floats[12] = 0.f; floats[13] = 0.f; floats[14] = 0.5f;

        return triangle_buffer;
    }

    bool VertexBuffer::edit()
    {
        char label[256];
        snprintf(label, 256, "%s###", m_name.c_str());
        if (!ImGui::CollapsingHeader(label))
        {
            return false;
        }
        imhelp::Indent indent;

        bool changed = false;
        if (imhelp::edit("Name", m_name))                     changed = true;
        if (imhelp::edit_list("Vertex Components", m_components))
        {
            m_data.resize(vertex_size() * m_num_vertices, 0);
            changed = true;
        }
        ImGui::Text("Vertices");
        if (ImGui::InputInt("Num Vertices", &m_num_vertices))
        {
            m_data.resize(vertex_size() * m_num_vertices, 0);
        }
        if (ImGui::IsItemDeactivatedAfterEdit())              changed = true;
        for (int i = 0; i < m_num_vertices; ++i)
        {
            ImGui::PushID(i);
            if (edit_vertex(i)) changed = true;
            ImGui::PopID();
            ImGui::Separator();
        }
        imhelp::display_error_if_present(m_error_log.c_str());

        return changed;
    }

    bool VertexBuffer::edit_vertex(int vertex_index)
    {
        if (m_components.empty())
        {
            return false;
        }

        bool changed = false;
        uint8_t* element = &(m_data[vertex_index * vertex_size()]);
        for (int component_index = 0; component_index < m_components.size(); ++component_index)
        {
            auto& component = m_components[component_index];

            ImGui::PushID(component_index);
            switch (component)
            {
            case gfx::VertexComponent::Float: if (imhelp::edit("", *reinterpret_cast<float*>(element)))         changed = true; break;
            case gfx::VertexComponent::Int:   if (imhelp::edit("", *reinterpret_cast<int*>(element)))           changed = true; break;
            case gfx::VertexComponent::Bool:  if (imhelp::edit("", *reinterpret_cast<bool*>(element)))          changed = true; break;
            case gfx::VertexComponent::Vec2:  if (imhelp::edit("", *reinterpret_cast<maths::Vector2*>(element))) changed = true; break;
            case gfx::VertexComponent::Vec3:  if (imhelp::edit("", *reinterpret_cast<maths::Vector3*>(element))) changed = true; break;
            }
            ImGui::PopID();

            element += gfx::component_size(component);
        }
        return changed;
    }

    int VertexBuffer::vertex_size() const
    {
        return gfx::vertex_size(m_components.data(), (int)m_components.size());
    }

    //ElementBuffer =================================================================

    static bool edit(const char* label, ElementBuffer::Triangle& vt)
    {
        int i3[3] = { (int)vt.a,(int)vt.b,(int)vt.c };
        ImGui::InputInt3(label, i3);
        vt = { (unsigned)i3[0], (unsigned)i3[1], (unsigned)i3[2] };

        return ImGui::IsItemDeactivatedAfterEdit();
    }

    bool ElementBuffer::edit()
    {
        char label[256];
        snprintf(label, 256, "%s###", m_name.c_str());
        if (!ImGui::CollapsingHeader(label))
        {
            return false;
        }
        imhelp::Indent indent;

        bool changed = false;
        if (imhelp::edit("Name", m_name))                changed = true;
        if (imhelp::edit_list("Triangles", m_triangles)) changed = true;
        imhelp::display_error_if_present(m_error_log.c_str());

        return changed;
    }

    //Shader ========================================================================

    template <unsigned shader_type>
    Shader<shader_type> Shader<shader_type>::create_triangle_shader() requires(shader_type == GL_VERTEX_SHADER)
    {
        Shader shader;
        shader.m_name = "triangle";
        shader.m_source = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoords;\n"
        "out vec2 texCoords;\n"
        "uniform float time;\n"
        "uniform mat4 camera;\n"
        "uniform mat4 transform;\n"
        "void main()\n"
        "{\n"
        //roatation part
        "    float x = aPos.x * cos(time) + aPos.y * sin(time);\n"
        "    float y = aPos.y * cos(time) - aPos.x * sin(time);\n"
        "    float z = aPos.z;\n"
        //outputs
        "    texCoords = aTexCoords;\n"
        "    gl_Position = camera * transform * vec4(x, y, z, 1.0);\n"
        "}\n"
        ;
        return shader;
    }

    template <unsigned shader_type>
    Shader<shader_type> Shader<shader_type>::create_triangle_shader() requires(shader_type == GL_FRAGMENT_SHADER)
    {
        Shader shader;
        shader.m_name = "triangle";
        shader.m_source = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 texCoords;\n"
        "uniform sampler2D tex;\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(tex, texCoords);\n"
        "}\n"
        ;
        return shader;
    }

    template <unsigned shader_type>
    bool Shader<shader_type>::edit()
    {
        char label[256];
        snprintf(label, 256, "%s###", m_name.c_str());
        if (!ImGui::CollapsingHeader(label))
        {
            return false;
        }
        imhelp::Indent indent;

        bool changed = false;
        if (imhelp::edit("Name", m_name))                      changed = true;
        if (imhelp::edit_multiline_string("Source", m_source)) changed = true;
        imhelp::display_error_if_present(m_error_log.c_str());

        return changed;
    }

    template class Shader<GL_VERTEX_SHADER>;
    template class Shader<GL_FRAGMENT_SHADER>;

    //ShaderProgram =================================================================

    ShaderProgram ShaderProgram::create_default_triangle_program()
    {
        ShaderProgram triangle_program;
        triangle_program.m_name = "triangle";
        triangle_program.m_vert_shader_name = "triangle";
        triangle_program.m_frag_shader_name = "triangle";
        return triangle_program;
    }

    bool ShaderProgram::edit()
    {
        char label[256];
        snprintf(label, 256, "%s###", m_name.c_str());
        if (!ImGui::CollapsingHeader(label))
        {
            return false;
        }
        imhelp::Indent indent;

        bool changed = false;
        if (imhelp::edit("Name", m_name))                        changed = true;
        if (imhelp::edit("Vertex Shader", m_vert_shader_name))   changed = true;
        if (imhelp::edit("Fragment Shader", m_frag_shader_name)) changed = true;
        imhelp::display_error_if_present(m_error_log.c_str());

        return changed;
    }

    //VertexArrayObject =============================================================

    VertexArrayObject VertexArrayObject::create_default_triangle_vao()
    {
        VertexArrayObject triangle_array;
        triangle_array.m_name = "triangle";
        triangle_array.m_vertex_buffer_name = "triangle";
        return triangle_array;
    }

    bool VertexArrayObject::edit()
    {
        char label[256];
        snprintf(label, 256, "%s###", m_name.c_str());
        if (!ImGui::CollapsingHeader(label))
        {
            return false;
        }
        imhelp::Indent indent;

        bool changed = false;
        if (imhelp::edit("Name", m_name))                          changed = true;
        if (imhelp::edit("Vertex buffer", m_vertex_buffer_name))   changed = true;
        if (imhelp::edit("Element buffer", m_element_buffer_name)) changed = true;
        imhelp::display_error_if_present(m_error_log.c_str());

        return changed;
    }

    //Texture =======================================================================

    Texture Texture::default_wall_texture()
    {
        Texture t;
        t.m_name = "triangle";
        t.m_filename = "wall.jpg";
        return t;
    }

    bool Texture::edit()
    {
        char label[256];
        snprintf(label, 256, "%s###", m_name.c_str());
        if (!ImGui::CollapsingHeader(label))
        {
            return false;
        }
        imhelp::Indent indent;

        bool changed = false;
        if (imhelp::edit("Name", m_name))         changed = true;
        if (imhelp::edit("Filename", m_filename)) changed = true;
        imhelp::display_error_if_present(m_error_log.c_str());

        return changed;
    }

    //GraphicsTestEditor ============================================================

    static bool edit(const char*, VertexBuffer& vb)       { return vb.edit(); }
    static bool edit(const char*, ElementBuffer& eb)      { return eb.edit(); }
    template<unsigned shader_type>
    static bool edit(const char*, Shader<shader_type>& s) { return s.edit(); }
    static bool edit(const char*, ShaderProgram& sp)      { return sp.edit(); }
    static bool edit(const char*, VertexArrayObject& vao) { return vao.edit(); }
    static bool edit(const char*, Texture& texture)       { return texture.edit(); }

    GraphicsTestEditor::GraphicsTestEditor()
    {
        m_data.m_vertex_buffers.push_back(VertexBuffer::create_triangle_buffer());
        m_data.m_vertex_shaders.push_back(VertexShader::create_triangle_shader());
        m_data.m_fragment_shaders.push_back(FragmentShader::create_triangle_shader());
        m_data.m_shader_programs.push_back(ShaderProgram::create_default_triangle_program());
        m_data.m_vertex_array_objects.push_back(VertexArrayObject::create_default_triangle_vao());
        m_data.m_textures.push_back(Texture::default_wall_texture());
    }

    bool GraphicsTestEditor::edit()
    {
        bool changed = m_deferred_update;
        m_deferred_update = false;

        if (ImGui::Begin("GraphicsTestEditor"))
        {
            static bool saving = false;
            if(ImGui::Button("Save state"))
            {
                saving = true;
                save_file_dialog({file::get_data_path(""), ".editor_state"});
            }
            ImGui::SameLine();
            if(ImGui::Button("Load state"))
            {
                saving = false;
                open_file_dialog({file::get_data_path(""), ".editor_state"});
            }
            auto update_dialog_result = update_file_dialog();
            if(update_dialog_result)
            {
                if(saving)
                {
                    auto file = file::FileOut::from_absolute(update_dialog_result->result_path.c_str());
                    m_data.write(file);
                }
                else
                {
                    auto file = file::FileIn::from_absolute(update_dialog_result->result_path.c_str());
                    m_data.read(file);
                    changed = true;
                }
            }

            ImGui::Text("Undo frame: %d, Undo/Redo length:[%d, %d]", m_undo_current, m_undo_length, m_redo_length);
            ImGui::Separator();
            if (imhelp::edit_list("Vertex Buffers", m_data.m_vertex_buffers))             changed = true;
            ImGui::Separator();
            if (imhelp::edit_list("Element Buffers", m_data.m_element_buffers))           changed = true;
            ImGui::Separator();
            if (imhelp::edit_list("Vertex shaders", m_data.m_vertex_shaders))             changed = true;
            ImGui::Separator();
            if (imhelp::edit_list("Fragment shaders", m_data.m_fragment_shaders))         changed = true;
            ImGui::Separator();
            if (imhelp::edit_list("Shader programs", m_data.m_shader_programs))           changed = true;
            ImGui::Separator();
            if (imhelp::edit_list("Vertex array objects", m_data.m_vertex_array_objects)) changed = true;
            ImGui::Separator();
            if (imhelp::edit_list("Textures", m_data.m_textures)) changed = true;

            //handle snapshot, undo, redo
            if (changed)
            {
                snapshot();
            }
            else if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_ModCtrl))
            {
                if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Z))
                {
                    if(undo()) changed = true;
                }
                if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Y))
                {
                    if(redo()) changed = true;
                }
            }
        }
        ImGui::End();

        return changed;
    }

    void GraphicsTestEditor::compile_assets(gfx::GraphicsManager &manager)
    {
        manager.clear();

        auto& v_buffers = m_data.m_vertex_buffers;
        auto& e_buffers = m_data.m_element_buffers;
        auto& v_shaders = m_data.m_vertex_shaders;
        auto& f_shaders = m_data.m_fragment_shaders;
        auto& programs  = m_data.m_shader_programs;
        auto& vaos      = m_data.m_vertex_array_objects;
        auto& textures  = m_data.m_textures;

        for(auto& buffer : v_buffers)
        {
            buffer.error_log().clear();
            manager.add(
                buffer.name().c_str(),
                std::make_unique<gfx::VertexBuffer>(buffer.data(), buffer.num_vertices(), buffer.components())
            );
        }
        gfx::report_gl_error();

        for(auto& buffer : e_buffers)
        {
            buffer.error_log().clear();
            manager.add(
                buffer.name().c_str(), 
                std::make_unique<gfx::ElementBuffer>(buffer.data(), buffer.num_triangles())
            );
        }
        gfx::report_gl_error();

        for(auto& vao : vaos)
        {
            vao.error_log().clear();

            //find components, report if missing
            bool requires_e_buffer = !vao.element_buffer_name().empty();
            auto* vbuffer = manager.vertex_buffer(vao.vertex_buffer_name().c_str());
            auto* ebuffer = manager.element_buffer(vao.element_buffer_name().c_str());
            vao.error_log() += vbuffer == nullptr ? "Couldn't find vertex buffer.\n" : "";
            vao.error_log() += requires_e_buffer && ebuffer == nullptr ? "Couldn't find element buffer.\n" : "";
            
            if (!vao.error_log().empty())
            {
                //no point trying to create a vao if we're missing the components
                continue;
            }

            manager.add(vao.name().c_str(), std::make_unique<gfx::VertexArray>(*vbuffer, ebuffer));
        }
        gfx::report_gl_error();

        for(auto& vert_shader : v_shaders)
        {
            vert_shader.error_log().clear();
            manager.add(
                vert_shader.name().c_str(), 
                std::make_unique<gfx::VertexShader>(vert_shader.source().c_str(), &vert_shader.error_log())
            );
        }
        gfx::report_gl_error();

        for(auto& frag_shader : f_shaders)
        {
            frag_shader.error_log().clear();
            manager.add(
                frag_shader.name().c_str(),
                std::make_unique<gfx::FragmentShader>(frag_shader.source().c_str(), &frag_shader.error_log())
            );
        }
        gfx::report_gl_error();

        for(auto& shader_program : programs)
        {
            shader_program.error_log().clear();
            
            //find components, report if missing
            auto* vshader = manager.vertex_shader(shader_program.vertex_shader().c_str());
            auto* fshader = manager.fragment_shader(shader_program.fragment_shader().c_str());
            shader_program.error_log() += vshader == nullptr ? "Couldn't find vertex shader.\n" : "";
            shader_program.error_log() += fshader == nullptr ? "Couldn't find fragment shader.\n" : "";

            if (!shader_program.error_log().empty())
            {
                //no point trying to create a valid shader program if we're missing the components
                continue;
            }

            manager.add(shader_program.name().c_str(), std::make_unique<gfx::ShaderProgram>(*vshader, *fshader, &shader_program.error_log()));

        }
        gfx::report_gl_error();

        for(auto& texture : textures)
        {
            texture.error_log().clear();

            if(texture.texture_filename().empty())
            {
                texture.error_log() = "Filename not specified.";
                continue;
            }

            gfx::Image texture_image(texture.texture_filename().c_str());
            if(!texture_image.valid())
            {
                texture.error_log() = "Couldn't load the file.";
            }
            manager.add(texture.name().c_str(), std::make_unique<gfx::Texture>(texture_image));
        }
        gfx::report_gl_error();
    }

    void GraphicsTestEditor::snapshot()
    {
        m_undo_current += 1;
        if (m_undo_current >= undo_stack_size) m_undo_current -= undo_stack_size;
        m_undo_length += 1;
        if (m_undo_length >= undo_stack_size) m_undo_length = undo_stack_size - 1;
        m_redo_length = 0;
        m_undo_stack[m_undo_current] = m_data;
    }

    bool GraphicsTestEditor::undo()
    {
        if (m_undo_length > 0)
        {
            m_undo_current -= 1;
            if (m_undo_current < 0) m_undo_current += undo_stack_size;
            m_undo_length -= 1;
            m_redo_length += 1;
            m_data = m_undo_stack[m_undo_current];
            return true;
        }
        return false;
    }

    bool GraphicsTestEditor::redo()
    {
        if (m_redo_length > 0)
        {
            m_undo_current += 1;
            if (m_undo_current >= undo_stack_size) m_undo_current -= undo_stack_size;
            m_undo_length += 1;
            m_redo_length -= 1;
            m_data = m_undo_stack[m_undo_current];
            return true;
        }
        return false;
    }
}
