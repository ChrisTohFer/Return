#include "return_engine/main_loop.h"

#include "physics/colliders.h"
#include "imgui/imgui.h"

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

void entity_3_update(re::Scene& scene, re::Entity& entity)
{
    //despawn at >100 units from origin
    if (entity.pos.magnitude_squared() > 10000.f) scene.remove_entity(entity);
}

int e1_cooldown = 60;
int e1_cooldown_variation = 5;
float e1_speed = 10.f;
float e1_speed_variation = 0.5f;
float e1_angle_variation = 0.f;
void entity_1_update(re::Scene& scene, re::Entity& entity)
{
    static int cooldown = e1_cooldown;
    --cooldown;
    if (cooldown <= 0)
    {
        cooldown = e1_cooldown + static_cast<int>(e1_cooldown_variation * (rand() / (RAND_MAX * 0.5f) - 1.f));
        re::Entity cube;
        cube.update_function = entity_3_update;
        
        cube.pos = entity.pos;
        float speed = e1_speed + e1_speed_variation * (rand() / (RAND_MAX * 0.5f) - 1.f);
        cube.rigid.velocity = entity.orientation * maths::Vector3{ 0.f, 0.f, speed };

        cube.rigid.angular_velocity = {rand() / (RAND_MAX * 0.5f), rand() / (RAND_MAX * 0.5f), rand() / (RAND_MAX * 0.5f)};

        cube.rigid.properties.gravity = 9.81f;
        
        cube.visual_component = std::make_unique<re::CubeComponent>();
        
        auto collider = std::make_unique<phys::Cuboid>();
        collider->min = -maths::Vector3::one() * 0.5f;
        collider->max = maths::Vector3::one() * 0.5f;
        cube.collider = std::move(collider);
        
        scene.add_entity(std::move(cube));
    }
}

int e2_cooldown = 60;
int e2_cooldown_variation = 5;
float e2_speed = 10.f;
float e2_speed_variation = 0.5f;
float e2_angle_variation = 0.f;
void entity_2_update(re::Scene& scene, re::Entity& entity)
{
    static int cooldown = e2_cooldown;
    --cooldown;
    if (cooldown <= 0)
    {
        cooldown = e2_cooldown + static_cast<int>(e2_cooldown_variation * (rand() / (RAND_MAX * 0.5f) - 1.f));
        re::Entity sphere;
        sphere.update_function = entity_3_update;

        sphere.pos = entity.pos;
        float speed = e2_speed + e2_speed_variation * (rand() / (RAND_MAX * 0.5f) - 1.f);
        sphere.rigid.velocity = entity.orientation * maths::Vector3{ 0.f, 0.f, speed };

        sphere.rigid.properties.gravity = 9.81f;

        sphere.visual_component = std::make_unique<re::SphereComponent>();

        auto collider = std::make_unique<phys::Sphere>();
        collider->radius = 1.f;
        sphere.collider = std::move(collider);

        scene.add_entity(std::move(sphere));
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
            e.update_function = entity_1_update;
            e.visual_component = std::make_unique<re::CubeComponent>();
            e.scale.z = 1.5f;

            e.pos.x -= 10.f;
            e.orientation = maths::Quaternion::from_euler({ -maths::PI * 0.25f, maths::PI * 0.5f, 0.f });
        }
        {
            m_entities.push_back(re::Entity{});
            auto& e = m_entities.back();
            e.visual_component = std::make_unique<re::SphereComponent>();
            e.pos.z -= 10.f;
            e.rigid.angular_velocity = {0.f, 1.f, 0.f};
        }
        {
            m_entities.push_back(re::Entity{});
            auto& e = m_entities.back();
            e.update_function = entity_2_update;
            e.visual_component = std::make_unique<re::CubeComponent>();
            e.scale.z = 1.5f;

            e.pos.x += 10.f;
            e.orientation = maths::Quaternion::from_euler({ -maths::PI * 0.25f, -maths::PI * 0.5f, 0.f });
        }
    }

    void on_update(float frame_time) override
    {
        //camera flight controls
        apply_camera_controls(m_camera, m_input_manager, frame_time);

        ImGui::Begin("Controls");
        ImGui::PushID(0);
        ImGui::SliderInt("Cooldown", &e1_cooldown, 30, 100);
        ImGui::SliderInt("Cooldown variation", &e1_cooldown_variation, 0, 30);
        ImGui::SliderFloat("Speed", &e1_speed, 5.f, 30.f);
        ImGui::SliderFloat("Speed variation", &e1_speed_variation, 0.f, 5.f);
        ImGui::PopID();
        ImGui::PushID(1);
        ImGui::SliderInt("Cooldown", &e2_cooldown, 30, 100);
        ImGui::SliderInt("Cooldown variation", &e2_cooldown_variation, 0, 30);
        ImGui::SliderFloat("Speed", &e2_speed, 5.f, 30.f);
        ImGui::SliderFloat("Speed variation", &e2_speed_variation, 0.f, 5.f);
        ImGui::PopID();

        ImGui::End();
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