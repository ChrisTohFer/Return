#include "scene.h"

#include "timer.h"

#include "maths/maths.h"

#include "editor_support/imgui_helpers.h"
#include "gfx/debug_lines.h"
#include "editor_support/file_dialog.h"

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
        m_dt = dt;
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
        
        Timer draw_timer;

        for(auto& entity : m_entities)
        {
            entity.visual_component->draw(entity.transform(), camera, *this, m_batch_renderer);
        }

        m_batch_renderer.draw_all((float)m_time, m_camera.view_matrix(), m_camera.projection_matrix());
        m_batch_renderer.clear();

        m_draw_time = draw_timer.age_seconds();
    }

    void Scene::editor_ui()
    {
        if(ImGui::Begin("Scene"))
        {
            static bool saving = false;
            if (ImGui::Button("Save"))
            {
                saving = true;
                save_file_dialog({ file::get_data_path(""), ".scene" });
            }
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                saving = false;
                open_file_dialog({ file::get_data_path(""), ".scene" });
            }

            auto result = update_file_dialog();
            if (result)
            {
                if (saving)
                {
                    auto file = file::FileOut::from_absolute(result->result_path.string().c_str());
                    write(file);
                }
                else
                {
                    auto file = file::FileIn::from_absolute(result->result_path.string().c_str());
                    read(file);
                    relink_assets();
                }
            }

            ImGui::Text("DT: %f", m_dt);
            ImGui::Text("Draw time: %f", m_draw_time);
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
            if (ImGui::Button("Paste"))
            {
                Entity e;
                e.pos = m_clipboard.pos;
                e.orientation = m_clipboard.orientation;
                e.scale = m_clipboard.scale;
                if (m_clipboard.visual_component)
                {
                    e.visual_component = m_clipboard.visual_component->clone();
                    e.visual_component->relink(*this);
                }
                m_entities.push_back(std::move(e));
            }
            ImGui::Checkbox("Show gizmos", &m_show_gizmos);
            ImGui::Text("Count: %d", (int)m_entities.size());

            if (ImGui::CollapsingHeader("Entities"))
            {
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
                    ImGui::SameLine();
                    if (ImGui::Button("Copy"))
                    {
                        m_clipboard.pos = entity.pos;
                        m_clipboard.orientation = entity.orientation;
                        m_clipboard.scale = entity.scale;
                        if (entity.visual_component)
                        {
                            m_clipboard.visual_component = entity.visual_component->clone();
                        }
                    }
                    ImGui::DragFloat3("Pos", &entity.pos.x, 0.1f);
                    ImGui::DragFloat3("Scale", &entity.scale.x, 0.1f);

                    auto transform = entity.transform();
                    if (m_show_gizmos && ImGuizmo::Manipulate(
                        m_camera.view_matrix().values,
                        m_camera.perspective_matrix().values,
                        ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE,
                        ImGuizmo::MODE::LOCAL,
                        transform.values))
                    {
                        entity.pos = transform.translation();
                        entity.orientation = maths::Quaternion::from_euler(transform.euler());
                    }

                    auto euler = entity.orientation.euler();
                    if (ImGui::DragFloat3("Rot", &euler.x, 0.1f))
                    {
                        entity.orientation = maths::Quaternion::from_euler(euler);
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
        }
        ImGui::End();
    }
    
    void Scene::relink_assets()
    {
        m_batch_renderer.clear(true);
        for(auto& entity : m_entities)
        {
            entity.visual_component->relink(*this);
        }
    }
}