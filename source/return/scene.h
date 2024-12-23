#pragma once

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

    struct Camera
    {
        maths::Vector3 pos = maths::Vector3(0.f, 0.f, 5.f);
        maths::Quaternion orientation = maths::Quaternion::identity();
        float fov_y = 1.f;
        float aspect = 1.f;
        float near = 0.001f;
        float far = 1000.f;

        //only for editor while there isn't a "Quaternion::to_euler" function
        maths::Vector3 euler = maths::Vector3::zero();

        maths::Matrix44 view_matrix();
        maths::Matrix44 perspective_matrix();
        maths::Matrix44 orthogonal_matrix();
    };

    struct Entity
    {
        maths::Vector3 pos = maths::Vector3::zero();
        maths::Vector3 scale = maths::Vector3::one();
        maths::Quaternion orientation = maths::Quaternion::identity();

        //only for editor while there isn't a "Quaternion::to_euler" function
        maths::Vector3 euler = maths::Vector3::zero();

        //vao
        const gfx::VertexArray* vao = nullptr;
        const gfx::ShaderProgram* program = nullptr;
        const gfx::Texture* texture = nullptr;

        std::string vao_name;
        std::string program_name;
        std::string texture_name;

        maths::Matrix44 transform() const;
    };

    class Scene
    {
    public:
        void update_and_draw(float dt, float aspect_ratio);

        void editor_ui(const gfx::GraphicsManager&);
        void relink_assets(const gfx::GraphicsManager&);

    private:
        std::vector<Entity> m_entities;
        const gfx::Texture* m_missing_texture;
        std::string m_missing_texture_name;
        double m_time = 0.0;
        Camera m_camera;
        bool m_perspective = true;
        DirectionalLight m_light;
        maths::Vector3 m_ambient_light = {0.4f,0.4f,0.4f};
    };


}