#pragma once

#include "gfx/gfx_forward.h"
#include "maths/maths.h"

#include <string>

namespace re
{
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
}