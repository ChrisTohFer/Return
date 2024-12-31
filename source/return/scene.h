#pragma once

#include "camera.h"
#include "entity.h"

#include "maths/maths.h"
#include "gfx/graphics_manager.h"

#include <vector>

namespace re
{
    struct DirectionalLight
    {
        maths::Vector3 direction = maths::Vector3(1.f,-1.f,-1.f).normalized();
        maths::Vector3 colour = {1.f,1.f,1.f};
    };

    class Scene
    {
    public:
        void update_and_draw(float dt, float aspect_ratio);

        void editor_ui(const gfx::GraphicsManager&);
        void relink_assets(const gfx::GraphicsManager&);

    private:
        std::vector<Entity> m_entities;
        double m_time = 0.0;
        
        OrbitCamera m_camera;
        bool m_perspective = true;

        const gfx::Texture* m_missing_texture;
        std::string m_missing_texture_name;
        DirectionalLight m_light;
        maths::Vector3 m_ambient_light = {0.4f,0.4f,0.4f};
    };


}