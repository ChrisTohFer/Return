#pragma once

#include "maths/maths.h"

#include <algorithm>

namespace phys
{
    struct AABB3
    {
        float min_x = 0.f;
        float max_x = 0.f;
        float min_y = 0.f;
        float max_y = 0.f;
        float min_z = 0.f;
        float max_z = 0.f;

        //clamp a vector to the bounds of this aabb
        maths::Vector3 clamp_point(maths::Vector3 vec) const;
    };

    struct Sphere
    {
        maths::Vector3 pos = maths::Vector3::zero();
        float radius = 0.f;
    };


    //function defs


    inline maths::Vector3 AABB3::clamp_point(maths::Vector3 vec) const
    {
        return {
            std::clamp(vec.x, min_x, max_x),
            std::clamp(vec.y, min_y, max_y),
            std::clamp(vec.z, min_z, max_z)
        };
    }

    
    //intent is that touching != intersecting

    inline bool intersects(const AABB3& a, const AABB3& b)
    {
        return 
            (a.max_x > b.min_x && a.min_x < b.max_x) &&
            (a.max_y > b.min_y && a.min_y < b.max_y) &&
            (a.max_z > b.min_z && a.min_z < b.max_z);
    }

    inline bool intersects(const Sphere& a, const Sphere& b)
    {
        const float radii_combined = a.radius + b.radius;
        const float radii_squared = radii_combined * radii_combined;
        const float separation_squared = (a.pos - b.pos).magnitude_squared();
        return separation_squared < radii_squared;
    }

    inline bool intersects(const Sphere& sphere, const AABB3& aabb)
    {
        //clamp a point on the aabb that is closest to the sphere
        const maths::Vector3 closest_point = aabb.clamp_point(sphere.pos);
        const float separation_squared = (closest_point - sphere.pos).magnitude_squared();
        const float radius_squared = sphere.radius * sphere.radius;
        return separation_squared < radius_squared;
    }

    inline bool intersects(const AABB3& aabb, const Sphere& sphere)
    {
        return intersects(sphere, aabb);
    }
}