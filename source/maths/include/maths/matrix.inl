#ifndef INCLUDED_MATHS_H
static_assert(false, "Don't include this file directly, it should be included via maths.h");
#endif

#include "maths.h"

namespace maths
{
    template<int rows, int columns>
    Matrix<rows, columns> operator*(const Matrix<rows, columns>& lhs, const Matrix<rows, columns>& rhs)
    {
        Matrix<rows, columns> result;

        for (int column = 0; column < columns; ++column)
        {
            for (int row = 0; row < rows; ++row)
            {
                result.get(row, column) =
                    lhs.get(row, 0) * rhs.get(0, column) +
                    lhs.get(row, 1) * rhs.get(1, column) +
                    lhs.get(row, 2) * rhs.get(2, column) +
                    lhs.get(row, 3) * rhs.get(3, column);
            }
        }

        return result;
    }
    template<int rows, int columns>
    Matrix<rows, columns> operator*(const Matrix<rows, columns>& lhs, float rhs)
    {
        Matrix<rows, columns> matrix;
        for (int i = 0; i < rows * columns; ++i)
        {
            matrix.values[i] = lhs.values[i] * rhs;
        }
        return matrix;
    }
    template<int rows, int columns>
    Matrix<rows, columns> operator*(float lhs, const Matrix<rows, columns>& rhs)
    {
        return rhs * lhs;
    }
    template<int rows, int columns>
    Matrix<rows, columns> operator/(const Matrix<rows, columns>& lhs, float rhs)
    {
        assert(rhs != 0.f);

        Matrix<rows, columns> matrix;
        for (int i = 0; i < rows * columns; ++i)
        {
            matrix.values[i] = lhs.values[i] / rhs;
        }
        return matrix;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::identity()
    {
        Matrix mat;
        for (int column = 0; column < columns; ++column)
        {
            for (int row = 0; row < rows; ++row)
            {
                mat.get(row, column) = (float)(row == column);
            }
        }

        return mat;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::from_scale(Vector3 v)
        requires(columns >= 3 && rows >= 3)
    {
        auto result = identity();
        result.get(0, 0) = v.x;
        result.get(1, 1) = v.y;
        result.get(2, 2) = v.z;
        return result;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::from_translation(Vector3 v)
        requires(columns >= 4 && rows >= 3)
    {
        auto result = identity();
        result.get(0, 3) = v.x;
        result.get(1, 3) = v.y;
        result.get(2, 3) = v.z;
        return result;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::from_orientation(Quaternion q)
        requires(columns >= 3 && rows >= 3)
    {
        Matrix result = identity();

        result.get(0, 0) = 2.f * (q.w * q.w + q.x * q.x) - 1;
        result.get(0, 1) = 2.f * (q.x * q.y - q.w * q.z);
        result.get(0, 2) = 2.f * (q.x * q.z + q.w * q.y);

        result.get(1, 0) = 2.f * (q.x * q.y + q.w * q.z);
        result.get(1, 1) = 2.f * (q.w * q.w + q.y * q.y) - 1;
        result.get(1, 2) = 2.f * (q.y * q.z - q.w * q.x);

        result.get(2, 0) = 2.f * (q.x * q.z - q.w * q.y);
        result.get(2, 1) = 2.f * (q.y * q.z + q.w * q.x);
        result.get(2, 2) = 2.f * (q.w * q.w + q.z * q.z) - 1;

        return result;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::from_x_rotation(float angle)
        requires(columns >= 3 && rows >= 3)
    {
        auto result = Matrix44::identity();

        result.get(0, 0) = 1.f;
        result.get(1, 0) = 0.f;
        result.get(2, 0) = 0.f;

        result.get(0, 1) = 0.f;
        result.get(1, 1) = cosf(angle);
        result.get(2, 1) = sinf(angle);

        result.get(0, 2) = 0.f;
        result.get(1, 2) = -sinf(angle);
        result.get(2, 2) = cosf(angle);

        return result;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::from_y_rotation(float angle)
        requires(columns >= 3 && rows >= 3)
    {
        auto result = Matrix44::identity();

        result.get(0, 0) = cosf(angle);
        result.get(1, 0) = 0.f;
        result.get(2, 0) = -sinf(angle);

        result.get(0, 1) = 0.f;
        result.get(1, 1) = 1.f;
        result.get(2, 1) = 0.f;

        result.get(0, 2) = sinf(angle);
        result.get(1, 2) = 0.f;
        result.get(2, 2) = cosf(angle);

        return result;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::from_z_rotation(float angle)
        requires(columns >= 3 && rows >= 3)
    {
        auto result = Matrix44::identity();

        result.get(0, 0) = cosf(angle);
        result.get(1, 0) = sinf(angle);
        result.get(2, 0) = 0.f;

        result.get(0, 1) = -sinf(angle);
        result.get(1, 1) = cosf(angle);
        result.get(2, 1) = 0.f;

        result.get(0, 2) = 0.f;
        result.get(1, 2) = 0.f;
        result.get(2, 2) = 1.f;

        return result;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::from_euler(Vector3 euler)
        requires(columns >= 3 && rows >= 3)
    {
        //todo: there's probably a more efficient way to compute the matrix, find out how!
        return
#ifdef EULER_ORDER_XYZ
            from_z_rotation(euler.z) *
            from_y_rotation(euler.y) *
            from_x_rotation(euler.x);
#elifdef EULER_ORDER_YXZ
            from_z_rotation(euler.z) *
            from_x_rotation(euler.x) *
            from_y_rotation(euler.y);
#elifdef EULER_ORDER_YZX
            from_x_rotation(euler.x) *
            from_z_rotation(euler.z) *
            from_y_rotation(euler.y);
#elifdef EULER_ORDER_XZY
            from_y_rotation(euler.y) *
            from_z_rotation(euler.z) *
            from_x_rotation(euler.x);
#elifdef EULER_ORDER_ZXY
            from_y_rotation(euler.y) *
            from_x_rotation(euler.x) *
            from_z_rotation(euler.z);
#elifdef EULER_ORDER_ZYX
            from_x_rotation(euler.x) *
            from_y_rotation(euler.y) *
            from_z_rotation(euler.z);
#else //default to zxy
            from_y_rotation(euler.y) *
            from_x_rotation(euler.x) *
            from_z_rotation(euler.z);
#endif
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::projection(float aspect, float fov, float near, float far)
        requires(columns == 4 && rows == 4)
    {
        Matrix44 result;

        float tan_fov_2 = tanf(fov / 2.f);

        result.get(0, 0) = 1.f / (aspect * tan_fov_2);
        result.get(0, 1) = 0.f;
        result.get(0, 2) = 0.f;
        result.get(0, 3) = 0.f;

        result.get(1, 0) = 0.f;
        result.get(1, 1) = 1.f / tan_fov_2;
        result.get(1, 2) = 0.f;
        result.get(1, 3) = 0.f;

        result.get(2, 0) = 0.f;
        result.get(2, 1) = 0.f;
        result.get(2, 2) = -(far + near) / (far - near);
        result.get(2, 3) = -2.f * far * near / (far - near);

        result.get(3, 0) = 0.f;
        result.get(3, 1) = 0.f;
        result.get(3, 2) = -1.f;
        result.get(3, 3) = 0.f;

        return result;
    }

    template<int rows, int columns>
    int Matrix<rows, columns>::index(int row, int col) const
    {
        return row + col * rows;
    }

    template<int rows, int columns>
    float Matrix<rows, columns>::get(int row, int col) const
    {
        return values[index(row, col)];
    }

    template<int rows, int columns>
    float& Matrix<rows, columns>::get(int row, int col)
    {
        return values[index(row, col)];
    }

    template<int rows, int columns>
    float Matrix<rows, columns>::determinant() const requires(rows == columns && rows != 1)
    {
        float result = 0;
        for (int row = 0; row < rows; ++row)
        {
            float sign = (row % 2 == 0) ? 1.f : -1.f;
            result += sign * get(row, 0) * submatrix(row, 0).determinant();
        }
        return result;
    }

    template<>
    inline float Matrix<1, 1>::determinant() const
    {
        return values[0];
    }

    template<int rows, int columns>
    Matrix<rows - 1, columns - 1> Matrix<rows, columns>::submatrix(int i, int j) const
    {
        Matrix<rows - 1, columns - 1> result;
        for (int column = 0; column < columns - 1; ++column)
        {
            int from_column = column < j ? column : column + 1;
            for (int row = 0; row < rows - 1; ++row)
            {
                int from_row = row < i ? row : row + 1;

                result.get(row, column) = get(from_row, from_column);
            }
        }

        return result;
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::adjugate() const
    {
        Matrix result;

        for (int column = 0; column < columns; ++column)
        {
            for (int row = 0; row < rows; ++row)
            {
                float sign = ((row + column) % 2 == 0) ? 1.f : -1.f;
                result.get(row, column) = sign * submatrix(row, column).determinant();
            }
        }

        return result.transpose();
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::inverse() const
    {
        return adjugate() / determinant();
    }

    template<int rows, int columns>
    Matrix<rows, columns> Matrix<rows, columns>::transpose() const
    {
        Matrix result;
        for (int row = 0; row < rows; ++row)
        {
            for (int column = 0; column < columns; ++column)
            {
                result.get(row, column) = get(column, row);
            }
        }
        return result;
    }

    template<int rows, int columns>
    Vector3 Matrix<rows, columns>::translation() const
        requires(columns >= 4 && rows >= 3)
    {
        return { get(0, 3), get(1, 3), get(2,3) };
    }

    template<int rows, int columns>
    Vector3 Matrix<rows, columns>::euler() const
        requires(columns >= 3 && rows >= 3)
    {
        //assumes rotation matrix is formed of RyRxRz (EULER_ORDER_ZXY)
        //definition taken from https://www.geometrictools.com/Documentation/EulerAngles.pdf
        if(get(1,2) < 1.f)
        {
            if(get(1,2) > -1.f)
            {
                return {
                    asinf(-get(1,2)),
                    atan2(get(0,2), get(2,2)),
                    atan2(get(1,0), get(1,1))
                };
            }
            else
            {
                return {
                    PI / 2.f,
                    -atan2(-get(0,1), get(0,0)),
                    0.f
                };
            }
        }
        else
        {
            return {
                -PI / 2.f,
                 atan2(-get(0,1) , get(0,0)),
                 0.f
            };
        }

    }
}