#pragma once

namespace gfx
{
    enum class VertexComponent
    {
        Float,
        Bool,
        Int,
        Vec2,
        Vec3
    };

    int component_size(VertexComponent);
    int vertex_size(const VertexComponent*, int n);
}