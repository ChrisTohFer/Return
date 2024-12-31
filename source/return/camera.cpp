#include "camera.h"

namespace re
{
    maths::Matrix44 Camera::view_matrix() const
    {
        return 
            maths::Matrix44::from_orientation(orientation.inverse()) *
            maths::Matrix44::from_translation(-pos);
    }

    maths::Matrix44 Camera::projection_matrix() const
    {
        return perspective ? perspective_matrix() : orthographic_matrix();
    }

    maths::Matrix44 Camera::perspective_matrix() const
    {
        return maths::Matrix44::projection(aspect, fov_y, near, far);
    }

    maths::Matrix44 Camera::orthographic_matrix() const
    {
        return 
            maths::Matrix44::from_scale(maths::Vector3(1.f/(fov_y*aspect), 1.f/fov_y, 1.f/far-near)) *
            maths::Matrix44::from_translation(maths::Vector3(0.f, 0.f, -near));
    }

    maths::Vector3 OrbitCamera::pos() const
    {
        return center + orientation * maths::Vector3(0.f, 0.f, orbit_distance);
    }

    maths::Matrix44 OrbitCamera::view_matrix() const
    {
        return 
            maths::Matrix44::from_orientation(orientation.inverse()) *
            maths::Matrix44::from_translation(-pos());
    }

    maths::Matrix44 OrbitCamera::projection_matrix() const
    {
        return perspective ? perspective_matrix() : orthographic_matrix();
    }

    maths::Matrix44 OrbitCamera::perspective_matrix() const
    {
        return maths::Matrix44::projection(aspect, fov_y, near, far);
    }

    maths::Matrix44 OrbitCamera::orthographic_matrix() const
    {
        return 
            maths::Matrix44::from_scale(maths::Vector3(1.f/(fov_y*aspect), 1.f/fov_y, 1.f/far-near)) *
            maths::Matrix44::from_translation(maths::Vector3(0.f, 0.f, -near));
    }
}