#pragma once

#include "visual_component.h"

#include "maths/maths.h"
#include "file/file.h"

#include <memory>

namespace re
{
    struct Entity
    {
        DEFINE_SERIALIZATION_FUNCTIONS(pos, scale, orientation, euler, visual_component);

        Entity()
        {
            visual_component = std::make_unique<VAOComponent>();
        }

        maths::Vector3 pos = maths::Vector3::zero();
        maths::Vector3 scale = maths::Vector3::one();
        maths::Quaternion orientation = maths::Quaternion::identity();

        //only for editor while there isn't a "Quaternion::to_euler" function
        maths::Vector3 euler = maths::Vector3::zero();

        std::unique_ptr<VisualComponent> visual_component;

        maths::Matrix44 transform() const;
    };
}