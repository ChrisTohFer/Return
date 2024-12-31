#pragma once

#include "maths/maths.h"

namespace re
{
    struct DirectionalLight
    {
        maths::Vector3 direction = maths::Vector3(1.f,-1.f,-1.f).normalized();
        maths::Vector3 colour = {1.f,1.f,1.f};
    };

    struct AmbientLight
    {
        maths::Vector3 colour = {0.4f, 0.4f, 0.4f};
    };
}