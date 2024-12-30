#pragma once

#include "maths/maths.h"
#include <vector>

namespace gfx
{
    void draw_line(
        const std::vector<maths::Vector3>& points,
        const maths::Matrix44& camera,
        maths::Vector3 colour,
        bool use_z,
        bool loop);
}