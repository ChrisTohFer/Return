#pragma once

#include "camera.h"
#include "entity.h"
#include "input_manager.h"

#include "gfx/batch_renderer.h"
#include "gfx/graphics_manager.h"

#include <vector>

namespace re
{
    class Scene
    {
    public:
        Scene(gfx::BatchRenderer& batch_renderer, const InputManager& input_manager)
            : m_batch_renderer(batch_renderer)
            , m_input_manager(input_manager)
        {
            m_entities.push_back(Entity{});
        }

        void try_update();
        void draw(float frame_time, float aspect_ratio);

    private:
        static constexpr float fixed_update_interval = 1.f / 60.f;

        gfx::BatchRenderer& m_batch_renderer;
        const InputManager& m_input_manager;
        Camera m_camera;
        float m_time_since_update = 0.f;

        std::vector<Entity> m_entities;
    };


}