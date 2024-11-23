#include "graphics_test2.h"

#include "glad/glad.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

bool edit_string(const char* label, std::string& s)
{
    bool changed = false;
    ImGui::InputText("Name", &s);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        changed = true;
    }
    return changed;
}

//VertexBuffer ==================================================================

bool VertexBuffer::edit()
{
    return edit_string("Name", m_name);
}

//Shader ========================================================================

template<unsigned shader_type>
bool Shader<shader_type>::edit()
{
    return edit_string("Name", m_name);
}

template Shader<GL_VERTEX_SHADER>;
template Shader<GL_FRAGMENT_SHADER>;

//ShaderProgram =================================================================

bool ShaderProgram::edit()
{
    return edit_string("Name", m_name);
}

//VertexArrayObject =============================================================

bool VertexArrayObject::edit()
{
    return edit_string("Name", m_name);
}

//GraphicsTestEditor ============================================================

template<typename ElementType>
bool edit_list_impl(const char* id, std::vector<ElementType>& list)
{
    bool changed = false;

    int to_delete = -1;
    int to_move = -1;

    ImGui::PushID(id);
    ImGui::SeparatorText(id);

    //iterate elements and give opportunity to edit each one
    for (int i = 0; i < list.size(); ++i)
    {
        ImGui::PushID(i);

        auto& element = list[i];
        if (ImGui::Button("-"))
        {
            to_delete = i;
        }
        ImGui::SameLine();
        if (ImGui::Button("^"))
        {
            to_move = i - 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("v"))
        {
            to_move = i;
        }
        ImGui::SameLine();
        
        char label[64];
        snprintf(label, sizeof(label), "%s###%d", element.name().c_str(), i);
        if (ImGui::CollapsingHeader(label))
        {
            if (element.edit())
            {
                changed = true;
            }
        }

        ImGui::PopID();
    }

    //handle delete request
    if (to_delete != -1)
    {
        list.erase(list.begin() + to_delete);
        changed = true;
    }

    //handle move request
    if (to_move >= 0 && to_move < (int)list.size() - 1)
    {
        std::swap(list[to_move], list[to_move + 1]);
        changed = true;
    }

    //allow adding new elements
    if (ImGui::Button("Add"))
    {
        list.push_back({});
        changed = true;
    }

    ImGui::PopID();

    return changed;
}

bool GraphicsTestEditor::edit()
{
    bool changed = false;
    if (ImGui::Begin("GraphicsTestEditor"))
    {
        ImGui::Text("Undo frame: %d, Undo/Redo length:[%d, %d]", m_undo_current, m_undo_length, m_redo_length);
        if (edit_list_impl("Buffers", m_data.m_vertex_buffers))                    changed = true;
        if (edit_list_impl("Vertex shaders", m_data.m_vertex_shaders))             changed = true;
        if (edit_list_impl("Fragment shaders", m_data.m_fragment_shaders))         changed = true;
        if (edit_list_impl("Shader programs", m_data.m_shader_programs))           changed = true;
        if (edit_list_impl("Vertex array objects", m_data.m_vertex_array_objects)) changed = true;

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
