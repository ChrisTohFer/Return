#pragma once

#include "constants.h"

#include <assert.h>
#include <cmath>
#include <math.h>

namespace maths
{
    struct Vector3;
    struct Quaternion;

    //class declarations ================================================================

    template<int rows, int columns>
    struct Matrix
    {
        float values[rows * columns];

        static Matrix identity();
        static Matrix from_scale(Vector3) requires(columns >= 3 && rows >= 3);
        static Matrix from_translation(Vector3) requires(columns >= 4 && rows >= 3);
        static Matrix from_orientation(Quaternion) requires(columns >= 3 && rows >= 3);
        static Matrix from_x_rotation(float) requires(columns >= 3 && rows >= 3);
        static Matrix from_y_rotation(float) requires(columns >= 3 && rows >= 3);
        static Matrix from_z_rotation(float) requires(columns >= 3 && rows >= 3);
        //euler angle order can be defined using EULER_ORDER_XYZ syntax, defaults to EULER_ORDER_ZXY
        static Matrix from_euler(Vector3) requires(columns >= 3 && rows >= 3);
        static Matrix projection(float aspect, float fov, float near, float far) requires(columns == 4 && rows == 4);

        int index(int row, int col) const;
        float get(int row, int col) const;
        float& get(int row, int col);

        float determinant() const requires(rows == columns && rows != 1);
        float determinant() const;
        Matrix<rows - 1, columns - 1> submatrix(int row, int col) const;
        Matrix adjugate() const;
        Matrix inverse() const;
        Matrix transpose() const;

        Vector3 translation() const requires(columns >= 4 && rows >= 3);
        Vector3 euler() const requires(columns >= 3 && rows >= 3);
    };
    using Matrix44 = Matrix<4, 4>;
    using Matrix34 = Matrix<3, 4>;

    struct Vector3
    {
        float x;
        float y;
        float z;

        //useful default values
        static constexpr Vector3 zero() { return { 0.f,0.f,0.f }; }
        static constexpr Vector3 one() { return { 1.f,1.f,1.f }; }
        static constexpr Vector3 unit_x() { return { 1.f,0.f,0.f }; }
        static constexpr Vector3 unit_y() { return { 0.f,1.f,0.f }; }
        static constexpr Vector3 unit_z() { return { 0.f,0.f,1.f }; }

        //operations
        //+-*/ operations are defined as non-member functions
        static float dot(const Vector3&, const Vector3&);
        static Vector3 cross(const Vector3&, const Vector3&);
        static Vector3 interpolate(const Vector3&, const Vector3&, float t);

        float magnitude_squared() const;
        float magnitude() const;
        Vector3 normalized() const;
    };

    struct Quaternion
    {
        float x;
        float y;
        float z;
        float w;

        static Quaternion identity() { return { 0,0,0,1.f }; }
        static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t);
        static Quaternion from_rotation_matrix(const Matrix44& mat);
        //euler angle order can be defined using EULER_ORDER_XYZ syntax, defaults to EULER_ORDER_ZXY
        static Quaternion from_euler(Vector3);

        Quaternion raised_to_power(float power) const;
        Quaternion normalized() const;
        Quaternion inverse() const;
        Vector3 axis() const;
        Vector3 axis_normalized() const;
        Vector3 euler() const;
        float angle() const;
        float mod_squared() const;
    };

    //operator declarations =============================================================

    //matrix
    template<int rows, int columns>
    Matrix<rows, columns> operator*(const Matrix<rows, columns>& lhs, const Matrix<rows, columns>& rhs);
    template<int rows, int columns>
    Matrix<rows, columns> operator*(const Matrix<rows, columns>& lhs, float rhs);
    template<int rows, int columns>
    Matrix<rows, columns> operator*(float lhs, const Matrix<rows, columns>& rhs);
    template<int rows, int columns>
    Matrix<rows, columns> operator/(const Matrix<rows, columns>& lhs, float rhs);

    //vector
    bool operator==(const Vector3& lhs, const Vector3& rhs);
    bool operator!=(const Vector3& lhs, const Vector3& rhs);

    Vector3 operator-(const Vector3& value);

    Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
    Vector3& operator+=(Vector3& lhs, const Vector3& rhs);

    Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
    Vector3& operator-=(Vector3& lhs, const Vector3& rhs);

    Vector3 operator*(const Vector3& lhs, float rhs);
    Vector3 operator*(float lhs, const Vector3& rhs);
    Vector3& operator*=(Vector3& lhs, float rhs);

    Vector3 operator/(const Vector3& lhs, float rhs);
    Vector3& operator/=(Vector3& lhs, float rhs);

    Vector3 operator*(const Matrix44& mat, Vector3 vec);
    Vector3 operator*(const Matrix34& mat, Vector3 vec);
    Vector3 operator*(const Quaternion& q, Vector3 vec);

    //quaternion
    bool operator==(const Quaternion& lhs, const Quaternion& rhs);
    bool operator!=(const Quaternion& lhs, const Quaternion& rhs);

    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
    Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);
}

//inline definitions ================================================================
#define INCLUDED_MATHS_H
#include "matrix.inl"
#include "vector3.inl"
#include "quaternion.inl"