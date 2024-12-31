#pragma once

namespace maths
{
    struct Vector2;
    struct Vector3;
    struct Quaternion;
    
    template<int rows, int columns>
    struct Matrix;
    using Matrix44 = Matrix<4, 4>;
    using Matrix34 = Matrix<3, 4>;
}