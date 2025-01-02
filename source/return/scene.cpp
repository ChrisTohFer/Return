#include "scene.h"

#include "maths/maths.h"

#include "editor_support/imgui_helpers.h"
#include "gfx/debug_lines.h"

#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"

#include "GLFW/glfw3.h"

namespace re
{
    Scene::Scene(const gfx::GraphicsManager& gfx_manager, const InputManager& input_manager)
        : m_gfx_manager(gfx_manager)
        , m_input_manager(input_manager)
    {}

    void Scene::update_and_draw(float dt, float aspect_ratio)
    {
        maths::Vector3 camera_movement = maths::Vector3::zero();
        if(m_input_manager.get_key(Key::W)) camera_movement.z -= dt * 5.f;
        if(m_input_manager.get_key(Key::A)) camera_movement.x -= dt * 5.f;
        if(m_input_manager.get_key(Key::S)) camera_movement.z += dt * 5.f;
        if(m_input_manager.get_key(Key::D)) camera_movement.x += dt * 5.f;
        if(m_input_manager.get_key(Key::E)) camera_movement.y += dt * 5.f;
        if(m_input_manager.get_key(Key::C)) camera_movement.y -= dt * 5.f;
        m_camera.pos += m_camera.orientation * camera_movement;

        if (m_input_manager.get_mouse_button(MouseButton::Right))
        {
            auto rotation = m_input_manager.mouse_delta() * 0.002f;
            m_camera.euler += {-rotation.y, -rotation.x};
            m_camera.orientation = maths::Quaternion::from_euler(m_camera.euler);
        }

        m_camera.aspect = aspect_ratio;
        m_time += dt;
        auto cam_projection = m_camera.projection_matrix();
        auto camera = cam_projection * m_camera.view_matrix();
        
        gfx::report_gl_error();
        
        for(auto& entity : m_entities)
        {
            entity.visual_component->draw(entity.transform(), camera, *this);
        }
    }

    void Scene::editor_ui()
    {
        if(ImGui::Begin("Scene"))
        {
            if (ImGui::BeginCombo("Missing texture", m_missing_texture_name.c_str()))
            {
                for (auto& texture : m_gfx_manager.texture_names())
                {
                    const bool selected = texture == m_missing_texture_name;
                    if (ImGui::Selectable(texture.c_str(), selected) && !selected)
                    {
                        m_missing_texture_name = texture;
                        m_missing_texture = m_gfx_manager.texture(texture.c_str());
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SeparatorText("Camera");
            ImGui::DragFloat3("Pos", &m_camera.pos.x, 0.1f);
            if (ImGui::DragFloat3("Rot", &m_camera.euler.x, 0.1f))
            {
                m_camera.orientation = maths::Quaternion::from_euler(m_camera.euler);
            }
            //ImGui::DragFloat("Orbit distance", &m_camera.orbit_distance, 0.1f);
            ImGui::DragFloat("fov_y", &m_camera.fov_y, 0.05f);
            ImGui::DragFloat("near", &m_camera.near);
            ImGui::DragFloat("far", &m_camera.far);
            ImGui::Checkbox("Perspective", &m_camera.perspective);

            ImGui::SeparatorText("Entities");
            int to_remove = -1;
            for(int i = 0; i < m_entities.size(); ++i)
            {
                auto& entity = m_entities[i];
                imhelp::Indent indentation;
                ImGuizmo::PushID(&entity);
                ImGui::PushID(&entity);
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

                edit("Test", entity.visual_component, *this);

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
    
    void Scene::relink_assets()
    {
        m_missing_texture = m_gfx_manager.texture(m_missing_texture_name.c_str());
        for(auto& entity : m_entities)
        {
            entity.visual_component->relink(*this);
        }
    }
}