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
        maths::Quaternion angular_velocity = maths::Quaternion{ 0.f, 0.5f, 0.f, 0.5f }.normalized();
        RigidBodyProperties properties;

        void update(float delta_time, maths::Vector3& position, maths::Quaternion& orientation);
    };

    inline void RigidBody::update(float delta_time, maths::Vector3& position, maths::Quaternion& orientation)
    {
        velocity.y -= properties.gravity * delta_time;
        
        position += velocity * delta_time;
        orientation *= maths::Quaternion::slerp(maths::Quaternion::identity(), angular_velocity, delta_time);
    }
}