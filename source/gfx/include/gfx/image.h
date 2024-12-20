#pragma once

namespace gfx
{
    class Image
    {
    public:
        Image(const char* path);
        Image(Image&&);
        ~Image();

        bool valid() const { return m_data != nullptr; }
        const void* data() const { return m_data; }
        int width() const { return m_width; }
        int height() const { return m_height; }
        int n_channels() const { return m_n_channels; }

    private:
        unsigned char* m_data = nullptr;
        int m_width      = 0;
        int m_height     = 0;
        int m_n_channels = 0;
        
    };
}