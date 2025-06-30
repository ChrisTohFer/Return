#include "return_engine/main_loop.h"


static void apply_camera_controls(re::Camera& camera, const re::InputManager& input_manager, float frame_time)
{
    {
        //position
        constexpr float speed = 5.f;
        auto camera_velocity = maths::Vector3::zero();
        if (input_manager.key_pressed(re::Key::W))        camera_velocity -= maths::Vector3::unit_z();
        if (input_manager.key_pressed(re::Key::S))        camera_velocity += maths::Vector3::unit_z();
        if (input_manager.key_pressed(re::Key::A))        camera_velocity -= maths::Vector3::unit_x();
        if (input_manager.key_pressed(re::Key::D))        camera_velocity += maths::Vector3::unit_x();
        if (input_manager.key_pressed(re::Key::Space))    camera_velocity += maths::Vector3::unit_y();
        if (input_manager.key_pressed(re::Key::LControl)) camera_velocity -= maths::Vector3::unit_y();

        camera_velocity = camera.orientation * camera_velocity;
        camera.pos += camera_velocity * frame_time * speed;
    }
    {
        //orientation
        auto angular_velocity = maths::Vector3::zero();
        if (input_manager.get_mouse_button(re::MouseButton::Right))
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

class SceneOverride : public re::Scene
{
public:
    SceneOverride(gfx::BatchRenderer& renderer, const re::InputManager& input_manager)
        : Scene(renderer, input_manager)
    {
        {
            m_entities.push_back(re::Entity{});
            auto& e = m_entities.back();
            e.visual_component = std::make_unique<re::SphereComponent>();
            auto sphere_collider = std::make_unique<phys::Sphere>();;
            sphere_collider->radius = 1.f;
            e.collider = std::move(sphere_collider);
        }
        {
            m_entities.push_back(re::Entity{});
            auto& e = m_entities.back();
            e.visual_component = std::make_unique<re::SphereComponent>();
            auto sphere_collider = std::make_unique<phys::Sphere>();;
            sphere_collider->radius = 1.f;
            e.collider = std::move(sphere_collider);

            e.pos.y += 10.f;
            e.rigid.properties.gravity = 1.f;
            e.rigid.properties.mass = 0.1f;
        }
    }

    void on_update(float frame_time) override
    {
        //camera flight controls
        apply_camera_controls(m_camera, m_input_manager, frame_time);
    }

private:
};

int main()
{
    //majority of functionality is handled by the engine itself hence calling straight into the engine main loop here
    //to customize the functionality of the engine for the game we use callbacks, virtual functions, and data
    const re::EngineConfig config;
    re::main_loop(config, [&](gfx::BatchRenderer& renderer, const re::InputManager& input_manager)
    {
        return std::make_unique<SceneOverride>(renderer, input_manager);
    });
}