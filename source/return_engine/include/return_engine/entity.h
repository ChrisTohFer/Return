#pragma once

#include "visual_component.h"

#include "maths/maths.h"
#include "physics/rigidbody.h"

#include <memory>

namespace re
{
    struct Entity
    {
        Entity()
        {
            visual_component = std::make_unique<SphereComponent>();
        }

        maths::Vector3 pos = maths::Vector3::zero();
        maths::Vector3 scale = maths::Vector3::one();
        maths::Quaternion orientation = maths::Quaternion::identity();

        phys::RigidBody rigid;

        std::unique_ptr<VisualComponent> visual_component;

        maths::Matrix44 transform() const;
    };
}