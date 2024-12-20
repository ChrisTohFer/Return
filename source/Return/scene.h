#pragma once

#include "maths/vector3.h"
#include "maths/quaternion.h"
#include "maths/matrix.h"
#include "gfx/graphics_manager.h"

#include <vector>

namespace re
{
    struct Camera
    {
        geom::Vector3 pos = geom::Vector3(0.f, 0.f, -5.f);
        geom::Quaternion orientation = geom::Quaternion::identity();
        float fov_y = 1.f;
        float aspect = 1.f;
        float near = 0.001f;
        float far = 1000.f;

        geom::Matrix44 view_matrix();
        geom::Matrix44 perspective_matrix();
        geom::Matrix44 orthogonal_matrix();
    };

    struct Entity
    {
        geom::Vector3 pos = geom::Vector3::zero();
        geom::Vector3 scale = geom::Vector3::one();
        geom::Quaternion orientation = geom::Quaternion::identity();

        //vao
        const gfx::VertexArray* vao = nullptr;
        const gfx::ShaderProgram* program = nullptr;
        const gfx::Texture* texture = nullptr;
    };

    class Scene
    {
    public:
        void update_and_draw(float dt, float aspect_ratio);
        void add_entity(const Entity&);

        void editor_ui(const gfx::GraphicsManager&);

    private:
        std::vector<Entity> m_entities;
        double m_time = 0.0;
        Camera m_camera;
        bool m_perspective = true;
    };


}