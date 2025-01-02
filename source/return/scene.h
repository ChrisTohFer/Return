#pragma once

#include "camera.h"
#include "entity.h"
#include "lights.h"
#include "input_manager.h"

#include "maths/maths.h"
#include "gfx/graphics_manager.h"

#include <vector>

namespace re
{
    class Scene
    {
    public:
        Scene(const gfx::GraphicsManager&, const InputManager&);
        void update_and_draw(float dt, float aspect_ratio);

        void editor_ui();
        void relink_assets();

        const DirectionalLight& directional_light() const { return m_light; }
        const AmbientLight& ambient_light() const { return m_ambient; }
        float time() const { return (float)m_time; }
        const gfx::GraphicsManager& gfx_manager() const { return m_gfx_manager; }

    private:
        std::vector<Entity> m_entities;
        double m_time = 0.0;
        
        OrbitCamera m_camera;

        const gfx::Texture* m_missing_texture;
        std::string m_missing_texture_name;
        DirectionalLight m_light;
        AmbientLight m_ambient;

        const gfx::GraphicsManager& m_gfx_manager;
        const InputManager& m_input_manager;
    };


}