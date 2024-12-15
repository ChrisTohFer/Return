#include "scene.h"

#include "maths/geometry.h"

#include "imgui/imgui.h"

#include "GLFW/glfw3.h"

namespace re
{
    geom::Matrix44 Camera::view_matrix()
    {
        return geom::create_rotation_matrix_from_quaternion(orientation.inverse()) * geom::create_translation_matrix_44(-pos);
    }

    geom::Matrix44 Camera::perspective_matrix()
    {
        return geom::create_projection_matrix_44(aspect, fov_y, 0.0001f, 1000.f);
    }

    void Scene::update_and_draw(float dt, float aspect_ratio)
    {
        m_camera.aspect = aspect_ratio;
        m_time += dt;
        auto camera = m_camera.perspective_matrix() * m_camera.view_matrix();

        for(auto& entity : m_entities)
        {
            if(entity.vao == nullptr || !entity.vao->valid()) continue;

            auto& vao = *entity.vao;
            glUseProgram(vao.program()->id());
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
            ImGui::DragFloat3("Camera", &m_camera.pos.x, 0.1f);

            ImGui::Text("Entities:");
            for(auto& entity : m_entities)
            {
                ImGui::PushID(&entity);
                ImGui::DragFloat3("Pos", &entity.pos.x, 0.1f);
                ImGui::PopID();
            }
        }
        ImGui::End();
    }
}