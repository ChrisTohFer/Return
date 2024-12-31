#include "texture.h"

#include "image.h"

#include "glad/glad.h"

namespace gfx
{
    Texture::Texture(const gfx::Image& image)
    {
        if(!image.valid())
        {
            return;
        }

        //create buffer
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
        //wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //filtering for magnify and minify
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, image.n_channels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, image.data());
        //generate mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    Texture::Texture(Texture&& other)
        : m_id(other.m_id)
    {
        other.m_id = 0;
    }
    Texture::~Texture()
    {
        if(m_id != 0)
            glDeleteTextures(1, &m_id);
    }
    
    void Texture::use() const
    {
        glBindTexture(GL_TEXTURE_2D, m_id);
    }
    
    void unbind_texture()
    {
        glBindTexture(GL_TEXTURE_2D, 0u);
    }
}