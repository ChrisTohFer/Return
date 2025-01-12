#pragma once

#include "maths/maths.h"
#include "file/file.h"

namespace re
{
    struct DirectionalLight
    {
        DEFINE_SERIALIZATION_FUNCTIONS(direction, colour);

        maths::Vector3 direction = maths::Vector3(1.f,-1.f,-1.f).normalized();
        maths::Vector3 colour = {1.f,1.f,1.f};
    };

    struct PointLight
    {
        DEFINE_SERIALIZATION_FUNCTIONS(position, colour);

        maths::Vector3 position = maths::Vector3::zero();
        maths::Vector3 colour = {1.f, 1.f, 1.f};
    };

    struct AmbientLight
    {
        DEFINE_SERIALIZATION_FUNCTIONS(colour);

        maths::Vector3 colour = {0.4f, 0.4f, 0.4f};
    };
}