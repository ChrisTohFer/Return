#pragma once

#include "maths/vector3.h"
#include "maths/quaternion.h"
#include "maths/matrix.h"
#include "graphics_components.h"

#include <vector>

namespace re
{
    struct Camera
    {
        geom::Vector3 pos = geom::Vector3(0.f, 0.f, -5.f);
        geom::Quaternion orientation = geom::Quaternion::identity();
        float fov_x = 2.f;

        geom::Matrix44 view_matrix();
        geom::Matrix44 perspective_matrix();
    };

    struct Entity
    {
        geom::Vector3 pos = geom::Vector3::zero();
        geom::Vector3 scale = geom::Vector3::one();
        geom::Quaternion orientation = geom::Quaternion::identity();

        //vao
        gfx::VertexArray* vao = nullptr;
    };

    class Scene
    {
    public:
        void update_and_draw(float dt);
        void add_entity(const Entity&);

        void editor_ui();

    private:
        std::vector<Entity> m_entities;
        double m_time = 0.0;
        Camera m_camera;
    };


}