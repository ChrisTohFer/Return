#include "graphics_test.h"

#include "imgui_helpers.h"

#include "glad/glad.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

//VertexBuffer ==================================================================

static bool edit(const char* label, ValueType& vt)
{
    int index = (int)vt;
    static const char* names[] = { "Float", "Int", "Bool", "Vec2", "Vec3", "Mat43" };
    bool changed = ImGui::Combo(label, &index, names, 6);
    vt = ValueType(index);
    return changed;
}

bool VertexBuffer::edit()
{
    bool changed = false;
    char label[256];
    snprintf(label, 256, "%s###", m_name.c_str());
    if (!ImGui::CollapsingHeader(label))
    {
        return changed;
    }
    if (imhelp::edit_string("Name", m_name)) changed = true;
    if (imhelp::edit_list("Components", m_components)) changed = true;
    return changed;
}

//Shader ========================================================================

template<unsigned shader_type>
bool Shader<shader_type>::edit()
{
    bool changed = false;
    char label[256];
    snprintf(label, 256, "%s###", m_name.c_str());
    if (!ImGui::CollapsingHeader(label))
    {
        return changed;
    }
    if (imhelp::edit_string("Name", m_name)) changed = true;
    if (imhelp::edit_multiline_string("Source", m_source)) changed = true;
    imhelp::display_error_if_present(m_error_log.c_str());
    return changed;
}

template Shader<GL_VERTEX_SHADER>;
template Shader<GL_FRAGMENT_SHADER>;

//ShaderProgram =================================================================

bool ShaderProgram::edit()
{
    char label[256];
    snprintf(label, 256, "%s###", m_name.c_str());
    if (!ImGui::CollapsingHeader(label))
    {
        return false;
    }
    return imhelp::edit_string("Name", m_name);
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
    return imhelp::edit_string("Name", m_name);
}

//GraphicsTestEditor ============================================================

static bool edit(const char* label, VertexBuffer& vb) { return vb.edit(); }
template<unsigned shader_type>
static bool edit(const char* label, Shader<shader_type>& s) { return s.edit(); }
static bool edit(const char* label, ShaderProgram& sp) { return sp.edit(); }
static bool edit(const char* label, VertexArrayObject& vao) { return vao.edit(); }

bool GraphicsTestEditor::edit()
{
    bool changed = false;
    if (ImGui::Begin("GraphicsTestEditor"))
    {
        ImGui::Text("Undo frame: %d, Undo/Redo length:[%d, %d]", m_undo_current, m_undo_length, m_redo_length);
        ImGui::SeparatorText("Buffers");
        if (imhelp::edit_list("Buffers", m_data.m_vertex_buffers))                    changed = true;
        ImGui::SeparatorText("Vertex shaders");
        if (imhelp::edit_list("Vertex shaders", m_data.m_vertex_shaders))             changed = true;
        ImGui::SeparatorText("Fragment shaders");
        if (imhelp::edit_list("Fragment shaders", m_data.m_fragment_shaders))         changed = true;
        ImGui::SeparatorText("Shader programs");
        if (imhelp::edit_list("Shader programs", m_data.m_shader_programs))           changed = true;
        ImGui::SeparatorText("Vertex array objects");
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
