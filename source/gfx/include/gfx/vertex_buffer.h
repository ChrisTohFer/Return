#pragma once

#include "gfx_forward.h"
#include "buffer_attributes.h"

#include <vector>

namespace gfx
{
    enum class PrimitiveType
    {
        None,
        Line,
        Triangle,
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(const void* data, int vertex_count, PrimitiveType, const std::vector<BufferAttributeType>& components);
        VertexBuffer(VertexBuffer&&);
        ~VertexBuffer();

        bool valid() const                       { return m_id != 0; }
        int vertex_count() const                 { return m_vertex_count; }
        PrimitiveType get_primitive_type() const { return m_primitive_type; }
        void bind_attributes() const;

    private:
        GLuint m_id = 0;
        int m_vertex_count = 0;
        PrimitiveType m_primitive_type;
        std::vector<BufferAttributeType> m_components;
    };
}