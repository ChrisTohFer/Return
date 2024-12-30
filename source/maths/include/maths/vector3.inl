#ifndef INCLUDED_MATHS_H
static_assert(false, "Don't include this file directly, it should be included via maths.h");
#endif

#include "maths.h"

namespace maths
{
    inline bool operator==(const Vector3& lhs, const Vector3& rhs)
    {
        return
            lhs.x == rhs.x &&
            lhs.y == rhs.y &&
            lhs.z == rhs.z;
    }

    inline bool operator!=(const Vector3& lhs, const Vector3& rhs)
    {
        return !(lhs == rhs);
    }

    inline Vector3 operator-(const Vector3& value)
    {
        return {
            -value.x,
            -value.y,
            -value.z
        };
    }

    inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs)
    {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z
        };
    }

    inline Vector3& operator+=(Vector3& lhs, const Vector3& rhs)
    {
        return lhs = lhs + rhs;
    }

    inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
    {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z
        };
    }

    inline Vector3& operator-=(Vector3& lhs, const Vector3& rhs)
    {
        return lhs = lhs - rhs;
    }

    inline Vector3 operator*(const Vector3& lhs, float rhs)
    {
        return {
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs
        };
    }

    inline Vector3 operator*(float lhs, const Vector3& rhs)
    {
        return {
            rhs.x * lhs,
            rhs.y * lhs,
            rhs.z * lhs
        };
    }

    inline Vector3& operator*=(Vector3& lhs, float rhs)
    {
        return lhs = lhs * rhs;
    }

    inline Vector3 operator/(const Vector3& lhs, float rhs)
    {
        assert(rhs != 0.f);

        return {
            lhs.x / rhs,
            lhs.y / rhs,
            lhs.z / rhs
        };
    }

    inline Vector3& operator/=(Vector3& lhs, float rhs)
    {
        return lhs = lhs / rhs;
    }

    inline float Vector3::dot(const Vector3& lhs, const Vector3& rhs)
    {
        return
            lhs.x * rhs.x +
            lhs.y * rhs.y +
            lhs.z * rhs.z;
    }

    inline Vector3 Vector3::cross(const Vector3& lhs, const Vector3& rhs)
    {
        return {
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        };
    }

    inline Vector3 Vector3::interpolate(const Vector3& v1, const Vector3& v2, float t)
    {
        return {
            std::lerp(v1.x, v2.x, t),
            std::lerp(v1.y, v2.y, t),
            std::lerp(v1.z, v2.z, t)
        };
    }
    
    inline float Vector3::angle_between(const Vector3& v1, const Vector3& v2) 
    {
        return acos(dot(v1, v2) / (v1.magnitude() * v2.magnitude()));
    }

    inline float Vector3::magnitude_squared() const
    {
        return x * x + y * y + z * z;
    }

    inline float Vector3::magnitude() const
    {
        return sqrtf(magnitude_squared());
    }

    inline Vector3 Vector3::normalized() const
    {
        float m = magnitude();
        if (m == 0.f)
        {
            return zero();
        }
        return *this / m;
    }
    
    inline Vector3 operator*(const Matrix44& mat, Vector3 vec)
    {
        Vector3 result;

        result.x =
            mat.get(0, 0) * vec.x +
            mat.get(0, 1) * vec.y +
            mat.get(0, 2) * vec.z +
            mat.get(0, 3);
        result.y =
            mat.get(1, 0) * vec.x +
            mat.get(1, 1) * vec.y +
            mat.get(1, 2) * vec.z +
            mat.get(1, 3);
        result.z =
            mat.get(2, 0) * vec.x +
            mat.get(2, 1) * vec.y +
            mat.get(2, 2) * vec.z +
            mat.get(2, 3);

        return result;
    }

    inline Vector3 operator*(const Matrix34& mat, Vector3 vec)
    {
        Vector3 result;

        result.x =
            mat.get(0, 0) * vec.x +
            mat.get(0, 1) * vec.y +
            mat.get(0, 2) * vec.z +
            mat.get(0, 3);
        result.y =
            mat.get(1, 0) * vec.x +
            mat.get(1, 1) * vec.y +
            mat.get(1, 2) * vec.z +
            mat.get(1, 3);
        result.z =
            mat.get(2, 0) * vec.x +
            mat.get(2, 1) * vec.y +
            mat.get(2, 2) * vec.z +
            mat.get(2, 3);

        return result;
    }

    inline Vector3 operator*(const Quaternion& q, Vector3 vec)
    {
        Quaternion q_res = q * Quaternion{ vec.x, vec.y, vec.z, 0.f } *q.inverse();
        return q_res.axis();
    }
}