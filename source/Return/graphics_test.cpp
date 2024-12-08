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

VertexBuffer VertexBuffer::create_triangle_buffer()
{
    VertexBuffer triangle_buffer;
    triangle_buffer.m_name = "triangle";
    triangle_buffer.m_components = { gfx::VertexComponent::Vec3 };
    triangle_buffer.m_num_vertices = 3;
    triangle_buffer.m_data.resize(3 * sizeof(geom::Vector3));

    auto* vertex = reinterpret_cast<geom::Vector3*>(triangle_buffer.m_data.data());
    vertex[0] = {-0.5f, -0.5f, 0.0f};
    vertex[1] = {0.5f, -0.5f, 0.0f};
    vertex[2] = {0.0f, 0.5f, 0.0f};

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
        case gfx::VertexComponent::Vec2:  if (imhelp::edit("", *reinterpret_cast<geom::Vector2*>(element))) changed = true; break;
        case gfx::VertexComponent::Vec3:  if (imhelp::edit("", *reinterpret_cast<geom::Vector3*>(element))) changed = true; break;
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
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
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
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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
    if (imhelp::edit_list("Uniforms", m_uniforms))         changed = true;
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
    triangle_array.m_shader_program_name = "triangle";
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
    if (imhelp::edit("Shader program", m_shader_program_name)) changed = true;
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

GraphicsTestEditor::GraphicsTestEditor()
{
    m_data.m_vertex_buffers.push_back(VertexBuffer::create_triangle_buffer());
    m_data.m_vertex_shaders.push_back(VertexShader::create_triangle_shader());
    m_data.m_fragment_shaders.push_back(FragmentShader::create_triangle_shader());
    m_data.m_shader_programs.push_back(ShaderProgram::create_default_triangle_program());
    m_data.m_vertex_array_objects.push_back(VertexArrayObject::create_default_triangle_vao());
}

bool GraphicsTestEditor::edit()
{
    bool changed = m_deferred_update;
    m_deferred_update = false;

    if (ImGui::Begin("GraphicsTestEditor"))
    {
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

//GraphicsTestPreview ===========================================================

void GraphicsTestPreview::initialize(GraphicsTestEditor& editor)
{
    m_compiled_vertex_buffers.clear();
    m_compiled_element_buffers.clear();
    m_compiled_vertex_shaders.clear();
    m_compiled_fragment_shaders.clear();
    m_compiled_shader_programs.clear();
    m_compiled_vaos.clear();

    gfx::report_gl_error_fatal();

    auto& v_buffers = editor.data().m_vertex_buffers;
    auto& e_buffers = editor.data().m_element_buffers;
    auto& v_shaders = editor.data().m_vertex_shaders;
    auto& f_shaders = editor.data().m_fragment_shaders;
    auto& programs = editor.data().m_shader_programs;
    auto& vaos = editor.data().m_vertex_array_objects;

    for(auto& buffer : v_buffers)
    {
        gfx::report_gl_error_fatal();
        buffer.error_log().clear();
        m_compiled_vertex_buffers.push_back(gfx::VertexBuffer(buffer.data(), buffer.num_vertices(), buffer.components()));
        auto e = glGetError();
        if(e != GL_NO_ERROR) buffer.error_log() = std::format("OpenGL error code {} when creating vertex buffer \"{}\".\n", e, buffer.name());
    }

    for(auto& buffer : e_buffers)
    {
        gfx::report_gl_error_fatal();
        buffer.error_log().clear();
        m_compiled_element_buffers.push_back(gfx::ElementBuffer(buffer.data(), buffer.num_triangles()));
        auto e = glGetError();
        if(e != GL_NO_ERROR) buffer.error_log() = std::format("OpenGL error code {} when creating vertex buffer \"{}\".\n", e, buffer.name());
    }

    for(auto& vert_shader : v_shaders)
    {
        gfx::report_gl_error_fatal();
        vert_shader.error_log().clear();
        m_compiled_vertex_shaders.push_back(gfx::VertexShader(vert_shader.source().c_str(), &vert_shader.error_log()));
    }
    
    for(auto& frag_shader : f_shaders)
    {
        gfx::report_gl_error_fatal();
        frag_shader.error_log().clear();
        m_compiled_fragment_shaders.push_back(gfx::FragmentShader(frag_shader.source().c_str(), &frag_shader.error_log()));
    }
    
    for(auto& shader_program : programs)
    {
        gfx::report_gl_error_fatal();
        shader_program.error_log().clear();
        
        //find components, report if missing
        auto find_vertex = std::find_if(v_shaders.begin(), v_shaders.end(), [&](auto& elem) { return shader_program.vertex_shader() == elem.name(); });
        auto find_fragment = std::find_if(f_shaders.begin(), f_shaders.end(), [&](auto& elem) { return shader_program.fragment_shader() == elem.name(); });
        shader_program.error_log() += find_vertex == v_shaders.end() ? "Couldn't find vertex shader.\n" : "";
        shader_program.error_log() += find_fragment == f_shaders.end() ? "Couldn't find fragment shader.\n" : "";

        if (!shader_program.error_log().empty())
        {
            //no point trying to create a valid shader program if we're missing the components
            m_compiled_shader_programs.push_back(gfx::ShaderProgram());
            continue;
        }
        const auto& vshader = m_compiled_vertex_shaders[find_vertex - v_shaders.begin()];
        const auto& fshader = m_compiled_fragment_shaders[find_fragment - f_shaders.begin()];
        m_compiled_shader_programs.push_back(gfx::ShaderProgram(vshader, fshader, &shader_program.error_log()));
        gfx::report_gl_error_fatal();
    }

    for(auto& vao : vaos)
    {
        gfx::report_gl_error_fatal();
        vao.error_log().clear();

        //find components, report if missing
        bool requires_e_buffer = !vao.element_buffer_name().empty();
        auto find_v_buffer = std::find_if(v_buffers.begin(), v_buffers.end(), [&](auto& elem) { return vao.vertex_buffer_name() == elem.name(); });
        auto find_e_buffer = std::find_if(e_buffers.begin(), e_buffers.end(), [&](auto& elem) { return vao.element_buffer_name() == elem.name(); });
        auto find_program = std::find_if(programs.begin(), programs.end(), [&](auto& elem) { return vao.program_name() == elem.name(); });
        vao.error_log() += find_v_buffer == v_buffers.end() ? "Couldn't find vertex buffer.\n" : "";
        vao.error_log() += requires_e_buffer && find_e_buffer == e_buffers.end() ? "Couldn't find element buffer.\n" : "";
        vao.error_log() += find_program == programs.end() ? "Couldn't find shader program.\n" : "";
        
        if (!vao.error_log().empty())
        {
            //no point trying to create a vao if we're missing the components
            m_compiled_vaos.push_back(gfx::VertexArray());
            continue;
        }
        const auto& v_buffer = m_compiled_vertex_buffers[find_v_buffer - v_buffers.begin()];
        const auto* e_buffer = requires_e_buffer ? &m_compiled_element_buffers[find_e_buffer - e_buffers.begin()] : nullptr;
        const auto& program = m_compiled_shader_programs[find_program - programs.begin()];

        m_compiled_vaos.push_back(gfx::VertexArray(v_buffer, program, e_buffer));
    }
}

void GraphicsTestPreview::draw() const
{
    if(ImGui::Begin("Preview"))
    {
        ImGui::Text("Buffers: ");
        for(auto& elem : m_compiled_vertex_buffers)
        {
            ImGui::Text("%d, ", (int)elem.id());
        }
        ImGui::Text("VShaders: ");
        for(auto& elem : m_compiled_vertex_shaders)
        {
            ImGui::Text("%d, ", (int)elem.id());
        }
        ImGui::Text("Shaders: ");
        for(auto& elem : m_compiled_fragment_shaders)
        {
            ImGui::Text("%d, ", (int)elem.id());
        }
        ImGui::Text("Programs: ");
        for(auto& elem : m_compiled_shader_programs)
        {
            ImGui::Text("%d, ", (int)elem.id());
        }
        ImGui::Text("Arrays: ");
        for(auto& elem : m_compiled_vaos)
        {
            ImGui::Text("%d, ", (int)elem.id());
        }
    }
    ImGui::End();
    for(auto& vao : m_compiled_vaos)
    {
        gfx::report_gl_error_fatal();
        vao.draw_triangles();
    }
}
