#pragma once

#include "maths/maths.h"

namespace phys
{
    struct RigidBodyProperties
    {
        float gravity = 0.f;
        float mass = 1.f;
    };

    struct RigidBody
    {
        maths::Vector3 velocity = maths::Vector3::zero();
        maths::Vector3 angular_velocity = maths::Vector3::zero();
        RigidBodyProperties properties;

        void update(float delta_time, maths::Vector3& position, maths::Quaternion& orientation);
    };

    inline void RigidBody::update(float delta_time, maths::Vector3& position, maths::Quaternion& orientation)
    {
        velocity.y -= properties.gravity * delta_time;
        
        position += velocity * delta_time;
        const float magnitude = angular_velocity.magnitude();
        if (magnitude != 0.f)
        {
            orientation *= maths::Quaternion::from_axis_and_rotation(angular_velocity / magnitude, delta_time * magnitude);
        }
    }
}