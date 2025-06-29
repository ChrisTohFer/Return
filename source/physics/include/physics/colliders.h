#pragma once

#include "maths/maths.h"

#include <algorithm>

namespace phys
{
    enum class ColliderType
    {
        Cuboid,
        Sphere,
        AABB,
    };

    struct Collider
    {
        virtual ~Collider() = default;
        virtual ColliderType type() const = 0;
    };

    struct AABB3 : public Collider
    {
        maths::Vector3 min = maths::Vector3::zero();
        maths::Vector3 max = maths::Vector3::zero();

        //clamp a vector to the bounds of this aabb
        maths::Vector3 clamp_point(maths::Vector3 vec) const;

        //collider interface
        ColliderType type() const override { return ColliderType::AABB; }
    };

    struct Sphere : public Collider
    {
        maths::Vector3 pos = maths::Vector3::zero();
        float radius = 0.f;

        //collider interface
        ColliderType type() const override { return ColliderType::Sphere; }
    };


    //function defs


    inline maths::Vector3 AABB3::clamp_point(maths::Vector3 vec) const
    {
        return {
            std::clamp(vec.x, min.x, max.x),
            std::clamp(vec.y, min.y, max.y),
            std::clamp(vec.z, min.z, max.z)
        };
    }

    
    //intent is that touching != intersecting

    inline bool intersects(const AABB3& a, const AABB3& b)
    {
        return 
            (a.max.x > b.min.x && a.min.x < b.max.x) &&
            (a.max.y > b.min.y && a.min.y < b.max.y) &&
            (a.max.z > b.min.z && a.min.z < b.max.z);
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