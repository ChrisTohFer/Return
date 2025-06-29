#pragma once

#include "visual_component.h"

#include "maths/maths.h"
#include "physics/colliders.h"
#include "physics/rigidbody.h"

#include <memory>

namespace re
{
    struct Entity
    {
        Entity()
        {
            visual_component = std::make_unique<SphereComponent>();
            auto sphere = std::make_unique<phys::Sphere>();
            sphere->radius = 1.f;
            collider = std::move(sphere);
        }

        maths::Vector3 pos = maths::Vector3::zero();
        maths::Vector3 scale = maths::Vector3::one();
        maths::Quaternion orientation = maths::Quaternion::identity();

        phys::RigidBody rigid;

        std::unique_ptr<VisualComponent> visual_component;
        std::unique_ptr<phys::Collider> collider;

        maths::Matrix44 transform() const;
    };
}