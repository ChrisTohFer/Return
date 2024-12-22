#include "scene.h"

#include "maths/maths.h"

#include "imgui_helpers.h"

#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"

#include "GLFW/glfw3.h"

namespace re
{
    maths::Matrix44 Camera::view_matrix()
    {
        return 
            maths::Matrix44::from_orientation(orientation.inverse()) *
            maths::Matrix44::from_translation(-pos);
    }

    maths::Matrix44 Camera::perspective_matrix()
    {
        return maths::Matrix44::projection(aspect, fov_y, near, far);
    }

    maths::Matrix44 Camera::orthogonal_matrix()
    {
        return 
            maths::Matrix44::from_scale(maths::Vector3(1.f/(fov_y*aspect), 1.f/fov_y, 1.f/far-near)) *
            maths::Matrix44::from_translation(maths::Vector3(0.f, 0.f, -near));
    }

    maths::Matrix44 Entity::transform() const
    {
        return 
            maths::Matrix44::from_translation(pos) *
            maths::Matrix44::from_orientation(orientation) *
            maths::Matrix44::from_scale(scale);
    }

    void Scene::update_and_draw(float dt, float aspect_ratio)
    {
        m_camera.aspect = aspect_ratio;
        m_time += dt;
        auto cam_projection = m_perspective ? m_camera.perspective_matrix() : m_camera.orthogonal_matrix();
        auto camera = cam_projection * m_camera.view_matrix();

        for(auto& entity : m_entities)
        {
            if(entity.vao == nullptr || entity.program == nullptr) continue;

            auto& vao = *entity.vao;
            auto& program = *entity.program;
                
            program.use();
            glBindVertexArray(vao.id());
            if(entity.texture)
            {
                entity.texture->use();
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            gfx::set_uniform(program.uniform_location("tex"), 0);
            gfx::set_uniform(program.uniform_location("time"), (float)m_time);
            gfx::set_uniform(program.uniform_location("transform"), entity.transform());
            gfx::set_uniform(program.uniform_location("camera"), camera);
            vao.draw_triangles();
        }
    }
    void Scene::add_entity(const Entity & e)
    {
        m_entities.push_back(e);
    }
    void Scene::editor_ui(const gfx::GraphicsManager& manager)
    {
        if(ImGui::Begin("Scene"))
        {
            ImGuizmo::BeginFrame();
            ImGuiIO& io = ImGui::GetIO();
            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
            
            auto vaos = manager.vertex_array_names();
            auto programs = manager.shader_program_names();
            auto textures = manager.texture_names();

            ImGui::DragFloat3("Camera pos", &m_camera.pos.x, 0.1f);
            if (ImGui::DragFloat3("Camera rot", &m_camera.euler.x, 0.1f))
            {
                m_camera.orientation = maths::Quaternion::from_euler(m_camera.euler);
            }
            ImGui::DragFloat("fov_y", &m_camera.fov_y, 0.05f);
            ImGui::DragFloat("near", &m_camera.near);
            ImGui::DragFloat("far", &m_camera.far);
            ImGui::Checkbox("Perspective", &m_perspective);

            ImGui::Text("Entities:");
            int to_remove = -1;
            for(int i = 0; i < m_entities.size(); ++i)
            {
                auto& entity = m_entities[i];
                imhelp::Indent indentation;
                ImGui::PushID(&entity);
                ImGuizmo::PushID(&entity);
                ImGui::Separator();
                if (ImGui::Button("Clear entity"))
                {
                    to_remove = i;
                }
                ImGui::DragFloat3("Pos", &entity.pos.x, 0.1f);
                ImGui::DragFloat3("Scale", &entity.scale.x, 0.1f);

                auto transform = entity.transform();
                if (ImGuizmo::Manipulate(
                    m_camera.view_matrix().values,
                    m_camera.perspective_matrix().values,
                    ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE,
                    ImGuizmo::MODE::LOCAL,
                    transform.values))
                {
                    entity.euler = transform.euler();
                    entity.pos = transform.translation();
                    entity.orientation = maths::Quaternion::from_euler(entity.euler);
                }
                if (ImGui::DragFloat3("Rot", &entity.euler.x, 0.1f))
                {
                    entity.orientation = maths::Quaternion::from_euler(entity.euler);
                }
                if (ImGui::Button("Clear vao"))
                {
                    entity.vao = nullptr;
                    entity.vao_name = "";
                }
                ImGui::SameLine();
                if (ImGui::BeginCombo("VAO", entity.vao_name.c_str()))
                {
                    for (auto& vao : vaos)
                    {
                        const bool selected = vao == entity.vao_name;
                        if (ImGui::Selectable(vao.c_str(), selected) && !selected)
                        {
                            entity.vao_name = vao;
                            entity.vao = manager.vertex_array(vao.c_str());
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Clear program"))
                {
                    entity.program = nullptr;
                    entity.program_name = "";
                }
                ImGui::SameLine();
                if (ImGui::BeginCombo("Program", entity.program_name.c_str()))
                {
                    for (auto& program : programs)
                    {
                        const bool selected = program == entity.program_name;
                        if (ImGui::Selectable(program.c_str(), selected) && !selected)
                        {
                            entity.program_name = program;
                            entity.program = manager.shader_program(program.c_str());
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Clear texture"))
                {
                    entity.texture = nullptr;
                    entity.texture_name = "";
                }
                ImGui::SameLine();
                if (ImGui::BeginCombo("Texture", entity.texture_name.c_str()))
                {
                    for (auto& texture : textures)
                    {
                        const bool selected = texture == entity.texture_name;
                        if (ImGui::Selectable(texture.c_str(), selected) && !selected)
                        {
                            entity.texture_name = texture;
                            entity.texture = manager.texture(texture.c_str());
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopID();
                ImGuizmo::PopID();
            }

            if (to_remove != -1)
            {
                m_entities.erase(m_entities.begin() + to_remove);
            }
            if (ImGui::Button("Add"))
            {
                m_entities.push_back({});
            }
        }
        ImGui::End();
    }
}