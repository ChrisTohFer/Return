#pragma once

#include <assert.h>
#include <cmath>
#include <math.h>

//might unify this with maths.h at some point but it's not really used for 3d geometry so might not be needed

namespace maths
{
    //struct

    struct Vector2
    {
        float x;
        float y;

        //useful default values
        static Vector2 zero() { return { 0.f,0.f }; }
        static Vector2 one() { return { 1.f,1.f }; }
        static Vector2 unit_x() { return { 1.f,0.f }; }
        static Vector2 unit_y() { return { 0.f,1.f }; }

        //operations
        //+-*/ operations are defined as non-member functions
        static float dot(const Vector2&, const Vector2&);
        static Vector2 interpolate(const Vector2&, const Vector2&, float t);

        float magnitude_squared() const;
        float magnitude() const;
        Vector2 normalized() const;
    };

    //operators

    bool operator==(const Vector2& lhs, const Vector2& rhs);
    bool operator!=(const Vector2& lhs, const Vector2& rhs);

    Vector2 operator-(const Vector2& value);

    Vector2 operator+(const Vector2& lhs, const Vector2& rhs);
    Vector2& operator+=(Vector2& lhs, const Vector2& rhs);

    Vector2 operator-(const Vector2& lhs, const Vector2& rhs);
    Vector2& operator-=(Vector2& lhs, const Vector2& rhs);

    Vector2 operator*(const Vector2& lhs, float rhs);
    Vector2 operator*(float lhs, const Vector2& rhs);
    Vector2& operator*=(Vector2& lhs, float rhs);

    Vector2 operator/(const Vector2& lhs, float rhs);
    Vector2& operator/=(Vector2& lhs, float rhs);
}

//inline definitions
namespace maths
{
    //inline operator definitions

    inline bool operator==(const Vector2& lhs, const Vector2& rhs)
    {
        return
            lhs.x == rhs.x &&
            lhs.y == rhs.y;
    }

    inline bool operator!=(const Vector2& lhs, const Vector2& rhs)
    {
        return !(lhs == rhs);
    }

    inline Vector2 operator-(const Vector2& value)
    {
        return {
            -value.x,
            -value.y
        };
    }

    inline Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
    {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }
    inline Vector2& operator+=(Vector2& lhs, const Vector2& rhs)
    {
        return lhs = lhs + rhs;
    }

    inline Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
    {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y
        };
    }
    inline Vector2& operator-=(Vector2& lhs, const Vector2& rhs)
    {
        return lhs = lhs - rhs;
    }

    inline Vector2 operator*(const Vector2& lhs, float rhs)
    {
        return {
            lhs.x * rhs,
            lhs.y * rhs
        };
    }
    inline Vector2 operator*(float lhs, const Vector2& rhs)
    {
        return {
            rhs.x * lhs,
            rhs.y * lhs
        };
    }
    inline Vector2& operator*=(Vector2& lhs, float rhs)
    {
        return lhs = lhs * rhs;
    }

    inline Vector2 operator/(const Vector2& lhs, float rhs)
    {
        assert(rhs != 0.f);

        return {
            lhs.x / rhs,
            lhs.y / rhs
        };
    }
    inline Vector2& operator/=(Vector2& lhs, float rhs)
    {
        return lhs = lhs / rhs;
    }

    //inline member definitions

    inline float Vector2::dot(const Vector2& lhs, const Vector2& rhs)
    {
        return
            lhs.x * rhs.x +
            lhs.y * rhs.y;
    }

    inline Vector2 Vector2::interpolate(const Vector2& v1, const Vector2& v2, float t)
    {
        return {
            std::lerp(v1.x, v2.x, t),
            std::lerp(v1.y, v2.y, t)
        };
    }

    inline float Vector2::magnitude_squared() const
    {
        return x * x + y * y;
    }

    inline float Vector2::magnitude() const
    {
        return sqrtf(magnitude_squared());
    }

    inline Vector2 Vector2::normalized() const
    {
        float m = magnitude();
        if (m == 0.f)
        {
            return zero();
        }
        return *this / m;
    }

}