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
        {}

        void try_update();
        void draw(float frame_time, float aspect_ratio);

        void add_entity(Entity entity);
        void remove_entity(Entity& entity);

        virtual void on_fixed_update() {}
        virtual void on_update(float /*frame_time*/) {}

    protected:
        float m_fixed_update_interval = 1.f / 60.f;
        float m_time_modifier         = 1.f;

        gfx::BatchRenderer& m_batch_renderer;
        const InputManager& m_input_manager;
        Camera              m_camera;
        float               m_time_since_update = 0.f;

        std::vector<Entity> m_entities;

        std::vector<Entity> m_entities_to_add;
        std::vector<size_t> m_entity_indices_to_remove;
    };


} // namespace re