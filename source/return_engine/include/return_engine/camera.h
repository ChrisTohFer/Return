#pragma once

#include "maths/maths.h"

#include "file/file.h"

namespace re
{
    struct Camera
    {
        maths::Vector3 pos = maths::Vector3(0.f, 0.f, 5.f);
        maths::Quaternion orientation = maths::Quaternion::identity();
        float fov_y = 1.f;
        float aspect = 1.f;
        float near = 0.001f;
        float far = 1000.f;
        bool  perspective = true;

        //only for editor while there isn't a "Quaternion::to_euler" function
        maths::Vector3 euler = maths::Vector3::zero();

        maths::Matrix44 view_matrix() const;
        maths::Matrix44 projection_matrix() const;
        maths::Matrix44 perspective_matrix() const;
        maths::Matrix44 orthographic_matrix() const;
    };

    struct OrbitCamera
    {
        maths::Vector3 center = maths::Vector3(0.f, 0.f, 0.f);
        maths::Quaternion orientation = maths::Quaternion::identity();
        float orbit_distance = 5.f;
        float fov_y = 1.f;
        float aspect = 1.f;
        float near = 0.001f;
        float far = 1000.f;
        bool  perspective = true;

        //only for editor while there isn't a "Quaternion::to_euler" function
        maths::Vector3 euler = maths::Vector3::zero();
        maths::Vector3 pos() const;

        maths::Matrix44 view_matrix() const;
        maths::Matrix44 projection_matrix() const;
        maths::Matrix44 perspective_matrix() const;
        maths::Matrix44 orthographic_matrix() const;
    };
}