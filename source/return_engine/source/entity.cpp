#include "entity.h"

namespace re
{
    maths::Matrix44 Entity::transform() const
    {
        return 
            maths::Matrix44::from_translation(pos) *
            maths::Matrix44::from_orientation(orientation) *
            maths::Matrix44::from_scale(scale);
    }
}