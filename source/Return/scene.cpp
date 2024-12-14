#include "scene.h"

#include "maths/geometry.h"

#include "imgui/imgui.h"

namespace re
{
    geom::Matrix44 Camera::view_matrix()
    {
        return geom::create_rotation_matrix_from_quaternion(orientation.inverse()) * geom::create_translation_matrix_44(-pos);
    }

    geom::Matrix44 Camera::perspective_matrix()
    {
        return geom::create_scale_matrix_44({ 1.f, 1.f, 0.001f });
    }

    void Scene::update_and_draw(float dt)
    {
        m_time += dt;
        auto camera = m_camera.perspective_matrix() * m_camera.view_matrix();

        for(auto& entity : m_entities)
        {
            if(entity.vao == nullptr) continue;

            auto& vao = *entity.vao;
            gfx::report_gl_error();
            glBindVertexArray(vao.id());
            gfx::set_uniform(vao.uniform_location("time"), (float)m_time);
            auto transform = geom::create_translation_matrix_44(entity.pos) *
                geom::create_rotation_matrix_from_quaternion(entity.orientation) *
                geom::create_scale_matrix_44(entity.scale);
            gfx::set_uniform(vao.uniform_location("transform"), transform);
            gfx::set_uniform(vao.uniform_location("camera"), camera);
            vao.draw_triangles();
        }
    }
    void Scene::add_entity(const Entity & e)
    {
        m_entities.push_back(e);
    }
    void Scene::editor_ui()
    {
        if(ImGui::Begin("Scene"))
        {
            ImGui::DragFloat3("Camera", &m_camera.pos.x);

            ImGui::Text("Entities:");
            for(auto& entity : m_entities)
            {
                ImGui::PushID(&entity);
                ImGui::DragFloat3("Pos", &entity.pos.x);
                ImGui::PopID();
            }
        }
        ImGui::End();
    }
}