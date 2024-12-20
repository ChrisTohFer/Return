#include "scene.h"

#include "maths/geometry.h"

#include "imgui/imgui.h"

#include "GLFW/glfw3.h"

namespace re
{
    geom::Matrix44 Camera::view_matrix()
    {
        return 
            geom::create_rotation_matrix_from_quaternion(orientation.inverse()) *
            geom::create_translation_matrix_44(-pos);
    }

    geom::Matrix44 Camera::perspective_matrix()
    {
        return geom::create_projection_matrix_44(aspect, fov_y, near, far);
    }

    geom::Matrix44 Camera::orthogonal_matrix()
    {
        return 
            geom::create_scale_matrix_44(geom::Vector3(1.f/(fov_y*aspect), 1.f/fov_y, 1.f/far-near)) *
            geom::create_translation_matrix_44(geom::Vector3(0.f, 0.f, -near));
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

            auto transform = geom::create_translation_matrix_44(entity.pos) *
                geom::create_rotation_matrix_from_quaternion(entity.orientation) *
                geom::create_scale_matrix_44(entity.scale);
            auto& vao = *entity.vao;
            auto& program = *entity.program;
                
            program.use();
            if(entity.texture)
            {
                entity.texture->use();
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(vao.id());
            gfx::set_uniform(program.uniform_location("tex"), 0);
            gfx::set_uniform(program.uniform_location("time"), (float)m_time);
            gfx::set_uniform(program.uniform_location("transform"), transform);
            gfx::set_uniform(program.uniform_location("camera"), camera);
            vao.draw_triangles();
        }
    }
    void Scene::add_entity(const Entity & e)
    {
        m_entities.push_back(e);
    }
    void Scene::editor_ui(const gfx::GraphicsManager&)
    {
        if(ImGui::Begin("Scene"))
        {
            ImGui::DragFloat3("Camera pos", &m_camera.pos.x, 0.1f);
            ImGui::DragFloat("fov_y", &m_camera.fov_y, 0.05f);
            ImGui::DragFloat("near", &m_camera.near);
            ImGui::DragFloat("far", &m_camera.far);
            ImGui::Checkbox("Perspective", &m_perspective);

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