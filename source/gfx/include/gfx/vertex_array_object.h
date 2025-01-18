#pragma once

#include "gfx_forward.h"

namespace gfx
{
    class VertexBuffer;
    class ElementBuffer;

    enum class PrimitiveType
    {
        None,
        Line,
        Triangle,
    };

    class VertexArray
    {
    public:
        VertexArray() = default;
        VertexArray(const VertexBuffer&, const ElementBuffer*, PrimitiveType);
        VertexArray(VertexArray&&);
        ~VertexArray();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        void draw() const;
        void draw(const VertexBuffer& instance_buffer) const;

    private:
        GLuint m_id = 0;
        const VertexBuffer* m_vb = nullptr;  //never nullptr
        const ElementBuffer* m_eb = nullptr; //sometimes nullptr
        PrimitiveType m_type;
    };
}