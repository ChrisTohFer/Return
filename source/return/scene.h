#pragma once

#include "camera.h"
#include "entity.h"
#include "gfx/lights.h"
#include "input_manager.h"

#include "maths/maths.h"
#include "gfx/batch_renderer.h"
#include "gfx/graphics_manager.h"

#include <vector>

namespace re
{
    class Scene
    {
    public:
        DEFINE_SERIALIZATION_FUNCTIONS(m_entities, m_time, m_camera, m_light, m_ambient);

        Scene(const gfx::GraphicsManager&, const InputManager&);
        void update_and_draw(float dt, float aspect_ratio);

        void editor_ui();
        void relink_assets();

        const DirectionalLight& directional_light() const { return m_light; }
        const AmbientLight& ambient_light() const { return m_ambient; }
        float time() const { return (float)m_time; }
        const gfx::GraphicsManager& gfx_manager() const { return m_gfx_manager; }

        gfx::BatchRenderer& batch_renderer() { return m_batch_renderer; }

    private:
        std::vector<Entity> m_entities;
        double m_time = 0.0;
        
        Camera m_camera;

        DirectionalLight m_light;
        AmbientLight m_ambient;

        Entity m_clipboard;
        bool m_show_gizmos = true;
        float m_dt;
        float m_draw_time;

        const gfx::GraphicsManager& m_gfx_manager;
        const InputManager& m_input_manager;
        gfx::BatchRenderer m_batch_renderer;
    };


}