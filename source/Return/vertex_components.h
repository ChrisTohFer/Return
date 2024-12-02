#pragma once

namespace gfx
{
    enum class VertexComponent
    {
        Float,
        Bool,
        Int,
        Vec2,
        Vec3,

        Num
    };
    const char* const g_vertex_component_names[] = { "Float", "Bool", "Int", "Vec2", "Vec3" };

    int component_size(VertexComponent);
    int vertex_size(const VertexComponent*, size_t n);
}