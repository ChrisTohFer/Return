#include "scene.h"

namespace re
{
    void apply_camera_controls(Camera& camera, const InputManager& input_manager, float frame_time)
    {
        {
            //position
            constexpr float speed = 5.f;
            auto camera_velocity = maths::Vector3::zero();
            if (input_manager.key_pressed(Key::W))        camera_velocity -= maths::Vector3::unit_z();
            if (input_manager.key_pressed(Key::S))        camera_velocity += maths::Vector3::unit_z();
            if (input_manager.key_pressed(Key::A))        camera_velocity -= maths::Vector3::unit_x();
            if (input_manager.key_pressed(Key::D))        camera_velocity += maths::Vector3::unit_x();
            if (input_manager.key_pressed(Key::Space))    camera_velocity += maths::Vector3::unit_y();
            if (input_manager.key_pressed(Key::LControl)) camera_velocity -= maths::Vector3::unit_y();

            camera_velocity = camera.orientation * camera_velocity;
            camera.pos += camera_velocity * frame_time * speed;
        }
        {
            //orientation
            auto angular_velocity = maths::Vector3::zero();
            if (input_manager.get_mouse_button(MouseButton::Right))
            {
                constexpr float rads_per_pixel = 0.001f;
                auto mouse_delta = input_manager.mouse_delta();

                angular_velocity.y += mouse_delta.x * rads_per_pixel;
                angular_velocity.x += mouse_delta.y * rads_per_pixel;
            }

            auto euler = camera.orientation.euler();
            euler += angular_velocity;
            camera.orientation = maths::Quaternion::from_euler(euler);
        }
    }

    void Scene::try_update()
    {
        if (m_time_since_update < fixed_update_interval)
        {
            return;
        }
        m_time_since_update -= fixed_update_interval;

        //update all entities
        for (auto& entity : m_entities)
        {
            entity.rigid.update(fixed_update_interval, entity.pos, entity.orientation);
        }
    }

    int draw_count = 0;
    void Scene::draw(float frame_time, float aspect_ratio)
    {
        m_time_since_update = fminf(m_time_since_update + frame_time, 2.f * fixed_update_interval);

        m_camera.aspect = aspect_ratio;

        //camera flight controls
        apply_camera_controls(m_camera, m_input_manager, frame_time);

        //draw all entities
        for (auto& entity : m_entities)
        {
            entity.visual_component->draw(entity.transform(), m_camera.perspective_matrix() * m_camera.view_matrix(), m_batch_renderer);
        }
        m_batch_renderer.draw_all(frame_time, m_camera.view_matrix(), m_camera.projection_matrix());
    }
}