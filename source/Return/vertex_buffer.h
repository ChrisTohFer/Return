#pragma once

#include "gfx_forward.h"
#include "vertex_components.h"

#include <vector>

namespace gfx
{
    class VertexBuffer
    {
    public:
        VertexBuffer(const void* data, int vertex_count, const std::vector<VertexComponent>& components);
        VertexBuffer(VertexBuffer&&);
        ~VertexBuffer();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        int vertex_count() const { return m_vertex_count; }
        void bind_attributes() const;
    private:
        GLuint m_id = 0;
        int m_vertex_count = 0;
        std::vector<VertexComponent> m_components;
    };
}