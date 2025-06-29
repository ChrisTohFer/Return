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
        maths::Vector3 pos = maths::Vector3::zero();
        maths::Vector3 scale = maths::Vector3::one();
        maths::Quaternion orientation = maths::Quaternion::identity();

        phys::RigidBody rigid;

        std::unique_ptr<VisualComponent> visual_component;
        std::unique_ptr<phys::Collider> collider;

        maths::Matrix44 transform() const;

        using UpdateCallback = void(*)(Entity& entity);
        UpdateCallback update_function = nullptr;
    };
}