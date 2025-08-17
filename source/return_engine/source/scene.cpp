#include "scene.h"

#include "imgui/imgui.h"
#include "physics/physics_system.h"

namespace re
{
    void Scene::try_update()
    {
        if (m_time_since_update < m_fixed_update_interval)
        {
            return;
        }
        m_time_since_update -= m_fixed_update_interval;

        on_fixed_update();

        //update all entities
        std::vector<phys::Entity> phys_entities;
        for (auto& entity : m_entities)
        {
            if (entity.update_function != nullptr) entity.update_function(*this, entity);
            phys_entities.emplace_back(entity.pos, entity.orientation, entity.collider.get(), &entity.rigid);
        }
        phys::solve_physics(phys_entities, m_fixed_update_interval);

        //update entities list

        //sort indices into descending order so that we don't invalidate the earlier indices before removing them
        std::sort(m_entity_indices_to_remove.rbegin(), m_entity_indices_to_remove.rend());
        for (auto index_to_remove : m_entity_indices_to_remove)
        {
            m_entities.erase(m_entities.begin() + index_to_remove);
        }
        m_entity_indices_to_remove.clear();

        for (auto& entity_to_add : m_entities_to_add)
        {
            m_entities.push_back(std::move(entity_to_add));
        }
        m_entities_to_add.clear();
    }

    void Scene::draw(float frame_time, float aspect_ratio)
    {
        if (ImGui::Begin("Time controls"))
        {
            float fps = 1.f / m_fixed_update_interval;
            if (ImGui::SliderFloat("Physics Fps", &fps, 1.f, 144.f))
            {
                if (m_fixed_update_interval < 1.f) m_fixed_update_interval = 1.f;
                m_fixed_update_interval = 1.f / fps;
            }
            ImGui::SliderFloat("Time scale", &m_time_modifier, 0.f, 2.f);
        }
        ImGui::End();
        frame_time *= m_time_modifier;


        m_time_since_update = fminf(m_time_since_update + frame_time, 2.f * m_fixed_update_interval);

        m_camera.aspect = aspect_ratio;

        on_update(frame_time);

        //draw all entities
        for (auto& entity : m_entities)
        {
            entity.visual_component->draw(entity.transform(), m_camera.perspective_matrix() * m_camera.view_matrix(), m_batch_renderer);
        }
        m_batch_renderer.draw_all(frame_time, m_camera.view_matrix(), m_camera.projection_matrix());
    }

    void Scene::add_entity(Entity entity)
    {
        m_entities_to_add.push_back(std::move(entity));
    }

    void Scene::remove_entity(Entity& entity)
    {
        assert(&entity >= m_entities.data() && &entity < m_entities.data() + m_entities.size());
        m_entity_indices_to_remove.push_back(&entity - m_entities.data());
    }
}