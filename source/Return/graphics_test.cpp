#include "graphics_test.h"

#include "imgui_helpers.h"

#include "maths/vector2.h"
#include "maths/vector3.h"

#include "glad/glad.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include <algorithm>
#include <format>

//VertexBuffer ==================================================================

static constexpr int value_type_size(ValueType vt)
{
    switch (vt)
    {
    case ValueType::Float: return sizeof(float);
    case ValueType::Int:   return sizeof(int);
    case ValueType::Bool:  return sizeof(bool);
    case ValueType::Vec2:  return sizeof(geom::Vector2);
    case ValueType::Vec3:  return sizeof(geom::Vector3);
    case ValueType::Mat43: return sizeof(float) * 12;
    }
    return -1;
}

static bool edit(const char* label, ValueType& vt)
{
    int index = (int)vt;
    static const char* names[] = { "Float", "Int", "Bool", "Vec2", "Vec3", "Mat43" };
    bool changed = ImGui::Combo(label, &index, names, 6);
    vt = ValueType(index);
    return changed;
}

static bool edit(const char* label, VertexBuffer::Triangle& vt)
{
    int i3[3] = { vt.a,vt.b,vt.c };
    ImGui::InputInt3(label, i3);
    vt = { i3[0], i3[1], i3[2] };

    return ImGui::IsItemDeactivatedAfterEdit();
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
    if (imhelp::edit_list("Triangles", m_triangles))      changed = true;
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
        case ValueType::Float: if (imhelp::edit("", *reinterpret_cast<float*>(element)))         changed = true; break;
        case ValueType::Int:   if (imhelp::edit("", *reinterpret_cast<int*>(element)))           changed = true; break;
        case ValueType::Bool:  if (imhelp::edit("", *reinterpret_cast<bool*>(element)))          changed = true; break;
        case ValueType::Vec2:  if (imhelp::edit("", *reinterpret_cast<geom::Vector2*>(element))) changed = true; break;
        case ValueType::Vec3:  if (imhelp::edit("", *reinterpret_cast<geom::Vector3*>(element))) changed = true; break;
        case ValueType::Mat43: break;
        }
        ImGui::PopID();

        element += value_type_size(component);
    }
    return changed;
}

int VertexBuffer::vertex_size() const
{
    int size = 0;
    for (auto& component : m_components)
    {
        size += value_type_size(component);
    }
    return size;
}

//Shader ========================================================================

template<unsigned shader_type>
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
    if (imhelp::edit_list("Uniforms", m_uniforms))         changed = true;
    if (imhelp::edit_multiline_string("Source", m_source)) changed = true;
    imhelp::display_error_if_present(m_error_log.c_str());

    return changed;
}

template class Shader<GL_VERTEX_SHADER>;
template class Shader<GL_FRAGMENT_SHADER>;

//ShaderProgram =================================================================

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
    if (imhelp::edit("Shader program", m_shader_program_name)) changed = true;
    imhelp::display_error_if_present(m_error_log.c_str());

    return changed;
}

//GraphicsTestEditor ============================================================

static bool edit(const char*, VertexBuffer& vb) { return vb.edit(); }
template<unsigned shader_type>
static bool edit(const char*, Shader<shader_type>& s) { return s.edit(); }
static bool edit(const char*, ShaderProgram& sp) { return sp.edit(); }
static bool edit(const char*, VertexArrayObject& vao) { return vao.edit(); }

bool GraphicsTestEditor::edit()
{
    bool changed = false;
    if (ImGui::Begin("GraphicsTestEditor"))
    {
        ImGui::Text("Undo frame: %d, Undo/Redo length:[%d, %d]", m_undo_current, m_undo_length, m_redo_length);
        ImGui::Separator();
        if (imhelp::edit_list("Buffers", m_data.m_vertex_buffers))                    changed = true;
        ImGui::Separator();
        if (imhelp::edit_list("Vertex shaders", m_data.m_vertex_shaders))             changed = true;
        ImGui::Separator();
        if (imhelp::edit_list("Fragment shaders", m_data.m_fragment_shaders))         changed = true;
        ImGui::Separator();
        if (imhelp::edit_list("Shader programs", m_data.m_shader_programs))           changed = true;
        ImGui::Separator();
        if (imhelp::edit_list("Vertex array objects", m_data.m_vertex_array_objects)) changed = true;

        //handle snapshot, undo, redo
        if (changed)
        {
            snapshot();
        }
        else if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_ModCtrl))
        {
            if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Z))
            {
                undo();
            }
            if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Y))
            {
                redo();
            }
        }
    }
    ImGui::End();

    return changed;
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

void GraphicsTestEditor::undo()
{
    if (m_undo_length > 0)
    {
        m_undo_current -= 1;
        if (m_undo_current < 0) m_undo_current += undo_stack_size;
        m_undo_length -= 1;
        m_redo_length += 1;
        m_data = m_undo_stack[m_undo_current];
    }
}

void GraphicsTestEditor::redo()
{
    if (m_redo_length > 0)
    {
        m_undo_current += 1;
        if (m_undo_current >= undo_stack_size) m_undo_current -= undo_stack_size;
        m_undo_length += 1;
        m_redo_length -= 1;
        m_data = m_undo_stack[m_undo_current];
    }
}

//GraphicsTestPreview ===========================================================

void GraphicsTestPreview::initialize(GraphicsTestEditor& editor)
{
    m_buffer_ids.clear();
    m_vertex_shader_ids.clear();
    m_fragment_shader_ids.clear();
    m_shader_program_ids.clear();
    m_vao_ids.clear();

    auto& buffers = editor.data().m_vertex_buffers;
    auto& v_shaders = editor.data().m_vertex_shaders;
    auto& f_shaders = editor.data().m_fragment_shaders;
    auto& programs = editor.data().m_shader_programs;
    //auto& vaos = editor.data().m_vertex_array_objects;
    
    {
        auto e = glGetError();
        if(e != GL_NO_ERROR) printf("OpenGL error code %d when initializing preview.\n", (int)e);
    }

    for(auto& buffer : buffers)
    {
        buffer.error_log().clear();
        GLuint id;
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, buffer.total_size(), buffer.data(), GL_STATIC_DRAW);
        auto e = glGetError();
        if(e != GL_NO_ERROR) buffer.error_log() = std::format("OpenGL error code {} when creating vertex buffer \"{}\".\n", e, buffer.name());
    }

    for(auto& vert_shader : v_shaders)
    {
        vert_shader.error_log().clear();
        
        auto id = glCreateShader(GL_VERTEX_SHADER);
        const char* source = vert_shader.source().c_str();
        glShaderSource(id, 1, &source, nullptr);
        glCompileShader(id);
        m_vertex_shader_ids.push_back(id);
        
        //check for and report errors
        int success = false;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            char buf[1024];
            glGetShaderInfoLog(id, sizeof(buf), nullptr, buf);
            vert_shader.error_log() = buf;
        }
    }
    
    for(auto& frag_shader : f_shaders)
    {
        frag_shader.error_log().clear();
        
        auto id = glCreateShader(GL_FRAGMENT_SHADER);
        const char* source = frag_shader.source().c_str();
        glShaderSource(id, 1, &source, nullptr);
        glCompileShader(id);
        m_fragment_shader_ids.push_back(id);
        
        //check for and report errors
        int success = false;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            char buf[1024];
            glGetShaderInfoLog(id, sizeof(buf), nullptr, buf);
            frag_shader.error_log() = buf;
        }
    }
    
    for(auto& shader_program : programs)
    {
        shader_program.error_log().clear();
        
        auto find_vertex = std::find_if(v_shaders.begin(), v_shaders.end(), [&](auto& elem) { return shader_program.vertex_shader() == elem.name(); });
        auto find_fragment = std::find_if(f_shaders.begin(), f_shaders.end(), [&](auto& elem) { return shader_program.fragment_shader() == elem.name(); });

        shader_program.error_log() += find_vertex == v_shaders.end() ? "Couldn't find vertex shader.\n" : "";
        shader_program.error_log() += find_fragment == f_shaders.end() ? "Couldn't find fragment shader.\n" : "";
        if (!shader_program.error_log().empty())
        {
            continue;
        }
        auto vertex_id = m_vertex_shader_ids[find_vertex - v_shaders.begin()];
        auto fragment_id = m_fragment_shader_ids[find_fragment - f_shaders.begin()];

        auto id = glCreateProgram();
        glAttachShader(id, vertex_id);
        glAttachShader(id, fragment_id);
        glLinkProgram(id);

        //check for and report errors
        int success = 0;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char buf[1024];
            glGetProgramInfoLog(id, sizeof(buf), nullptr, buf);
            shader_program.error_log() = buf;
        }

        m_shader_program_ids.push_back(id);
    }

}
