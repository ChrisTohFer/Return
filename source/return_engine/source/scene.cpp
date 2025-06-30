#include "scene.h"

#include "physics/physics_system.h"

namespace re
{
    void Scene::try_update()
    {
        if (m_time_since_update < fixed_update_interval)
        {
            return;
        }
        m_time_since_update -= fixed_update_interval;

        on_fixed_update();

        //update all entities
        std::vector<phys::Entity> phys_entities;
        for (auto& entity : m_entities)
        {
            if (entity.update_function != nullptr) entity.update_function(entity);
            phys_entities.emplace_back(entity.pos, entity.orientation, entity.collider.get(), &entity.rigid);
        }
        phys::solve_physics(phys_entities, fixed_update_interval);
    }

    void Scene::draw(float frame_time, float aspect_ratio)
    {
        m_time_since_update = fminf(m_time_since_update + frame_time, 2.f * fixed_update_interval);

        m_camera.aspect = aspect_ratio;

        on_update(frame_time);

        //draw all entities
        for (auto& entity : m_entities)
        {
            entity.visual_component->draw(entity.transform(), m_camera.perspective_matrix() * m_camera.view_matrix(), m_batch_renderer);
        }
        m_batch_renderer.draw_all(frame_time, m_camera.view_matrix(), m_camera.projection_matrix());
    }
}