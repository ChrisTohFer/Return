#pragma once

#include "gfx_forward.h"

namespace gfx
{
    class VertexBuffer;
    class ElementBuffer;

    class VertexArray
    {
    public:
        VertexArray() = default;
        VertexArray(const VertexBuffer&, const ElementBuffer*);
        VertexArray(VertexArray&&);
        ~VertexArray();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        void draw_triangles(const VertexBuffer* instance_buffer = nullptr) const;
        void draw_lines(const VertexBuffer* instance_buffer = nullptr) const;
        void use() const;

    private:
        GLuint m_id = 0;
        const VertexBuffer* m_vb = nullptr;  //never nullptr
        const ElementBuffer* m_eb = nullptr; //sometimes nullptr
    };
}