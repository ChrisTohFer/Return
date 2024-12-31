#pragma once

#include "gfx_forward.h"

namespace gfx
{
    class Image;

    class Texture
    {
    public:
        Texture(const gfx::Image&);
        Texture(Texture&&);
        ~Texture();

        bool valid() const { return m_id != 0; }
        GLuint id() const { return m_id; }
        void use() const;

    private:
        GLuint m_id;
    };

    void unbind_texture();
}