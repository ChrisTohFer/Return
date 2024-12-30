#pragma once

#include "maths/maths.h"
#include <vector>

namespace gfx
{
    void draw_line(
        const std::vector<maths::Vector3>& points,
        const maths::Matrix44& camera,
        maths::Vector3 colour,
        bool use_z = true,
        bool loop = false);
    
    void draw_sphere(
        maths::Vector3 pos,
        float radius,
        const maths::Matrix44& camera,
        maths::Vector3 colour,
        int num_segments = 20,
        bool use_z = true);
    
    //TODO!
    void draw_frustum(
        maths::Vector3 pos,
        float fov_y,
        float near,
        float far,
        const maths::Matrix44& view,
        const maths::Matrix44& projection,
        maths::Vector3 colour,
        bool use_z = true);
    
    void draw_aabb(
        maths::Vector3 min,
        maths::Vector3 max,
        const maths::Matrix44& camera,
        maths::Vector3 colour,
        bool use_z = true);
}