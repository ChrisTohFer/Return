#include "return_engine/main_loop.h"

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