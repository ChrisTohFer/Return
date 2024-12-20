#include "image.h"

#include "file/file.h"
#include "stb_image/stb_image.h"

namespace gfx
{
    Image::Image(const char* path)
    {
        m_data = stbi_load(file::get_data_path(path).c_str(), &m_width, &m_height, &m_n_channels, 0);
    }
    Image::Image(Image&& other)
        : m_data(other.m_data)
        , m_width(other.m_width)
        , m_height(other.m_height)
        , m_n_channels(other.m_n_channels)
    {
        other.m_data = nullptr;
        other.m_width = 0;
        other.m_height = 0;
        other.m_n_channels = 0;
    }
    Image::~Image()
    {
        if(m_data != nullptr)
            stbi_image_free(m_data);
    }
}