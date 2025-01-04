#include "file.h"

#include "maths/maths.h"
#include "maths/vector2.h"

#include <assert.h>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace file
{
    std::filesystem::path get_data_path(const char* relative_path)
    {
        auto result = std::filesystem::current_path();
        result += "/data/";
        result += relative_path;
        return result;
    }
    std::filesystem::path get_appdata_path(const char* /*relative_path*/)
    {
        assert(!"get_appdata_path() not implemented yet!");
        return std::filesystem::path();
    }
    void create_missing_directories(const std::filesystem::path& path)
    {
        auto parent_path = path.parent_path();
        if (!std::filesystem::exists(parent_path))
        {
            std::filesystem::create_directories(parent_path);
        }
    }

    //FileOut =========================================================================

    struct FileOut::Impl
    {
        std::ofstream file;
    };
    FileOut FileOut::from_data(const char *relative_path)
    {
        auto path = get_data_path(relative_path);
        create_missing_directories(path);
        return FileOut(path.string().c_str());
    }
    FileOut FileOut::from_app_data(const char *relative_path)
    {
        auto path = get_appdata_path(relative_path);
        create_missing_directories(path);
        return FileOut(path.string().c_str());
    }
    FileOut FileOut::from_absolute(const char *absolute_path)
    {
        auto path = std::filesystem::path(absolute_path);
        create_missing_directories(path);
        return FileOut(path.string().c_str());
    }
    FileOut::~FileOut() = default;

    FileOut::FileOut(const char *path)
    {
        m_impl = std::make_unique<Impl>();
        m_impl->file.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
    }

    bool FileOut::valid() const
    {
        return m_impl->file.good();
    }

    FileOut& FileOut::operator<<(const int8_t& value)     { write(value); return *this; }
    FileOut& FileOut::operator<<(const int16_t& value)    { write(value); return *this; }
    FileOut& FileOut::operator<<(const int32_t& value)    { write(value); return *this; }
    FileOut& FileOut::operator<<(const int64_t& value)    { write(value); return *this; }
    FileOut& FileOut::operator<<(const uint8_t& value)    { write(value); return *this; }
    FileOut& FileOut::operator<<(const uint16_t& value)   { write(value); return *this; }
    FileOut& FileOut::operator<<(const uint32_t& value)   { write(value); return *this; }
    FileOut& FileOut::operator<<(const uint64_t& value)   { write(value); return *this; }
    FileOut& FileOut::operator<<(const float& value)      { write(value); return *this; }
    FileOut& FileOut::operator<<(const double& value)     { write(value); return *this; }
    FileOut& FileOut::operator<<(const bool& value)       { write(value); return *this; }
    FileOut& FileOut::operator<<(const std::string& value)
    {
        size_t size = value.size();
        write(size);
        write(value.data(), size);
        return *this;
    }
    FileOut& FileOut::operator<<(const char* value)
    {
        size_t size = strlen(value);
        write(size);
        write(value, size);
        return *this;
    }

    FileOut& FileOut::operator<<(const maths::Vector2& value)    { write(value); return *this; }
    FileOut& FileOut::operator<<(const maths::Vector3& value)    { write(value); return *this; }
    FileOut& FileOut::operator<<(const maths::Quaternion& value) { write(value); return *this; }
    FileOut& FileOut::operator<<(const maths::Matrix34& value)   { write(value); return *this; }
    FileOut& FileOut::operator<<(const maths::Matrix44& value)   { write(value); return *this; }

    void FileOut::write(const void* data, size_t size)
    {
        m_impl->file.write(reinterpret_cast<const char*>(data), size);
    }

    template<typename T>
    void FileOut::write(const T& value)
    {
        m_impl->file.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    //FileIn ==========================================================================

    struct FileIn::Impl
    {
        std::ifstream file;
    };
    FileIn FileIn::from_data(const char* relative_path)
    {
        return FileIn(get_data_path(relative_path).string().c_str());
    }
    FileIn FileIn::from_app_data(const char *relative_path)
    {
        return FileIn(get_appdata_path(relative_path).string().c_str());
    }
    FileIn FileIn::from_absolute(const char *path)
    {
        return FileIn(path);
    }
    FileIn::~FileIn() = default;

    FileIn::FileIn(const char* path)
    {
        m_impl = std::make_unique<Impl>();
        if(std::filesystem::exists(path))
        {
            m_impl->file.open(path, std::ios::binary | std::ios::in);
        }
    }

    bool FileIn::valid() const
    {
        return m_impl->file.good();
    }

    FileIn& FileIn::operator>>(int8_t& value)   { read(value); return *this; }
    FileIn& FileIn::operator>>(int16_t& value)  { read(value); return *this; }
    FileIn& FileIn::operator>>(int32_t& value)  { read(value); return *this; }
    FileIn& FileIn::operator>>(int64_t& value)  { read(value); return *this; }
    FileIn& FileIn::operator>>(uint8_t& value)  { read(value); return *this; }
    FileIn& FileIn::operator>>(uint16_t& value) { read(value); return *this; }
    FileIn& FileIn::operator>>(uint32_t& value) { read(value); return *this; }
    FileIn& FileIn::operator>>(uint64_t& value) { read(value); return *this; }
    FileIn& FileIn::operator>>(float& value)    { read(value); return *this; }
    FileIn& FileIn::operator>>(double& value)   { read(value); return *this; }
    FileIn& FileIn::operator>>(bool& value)     { read(value); return *this; }
    FileIn& FileIn::operator>>(std::string& value)
    {
        size_t size;
        if (read(size))
        {
            value.resize(size);
            read(value.data(), size);
        }
        return *this;
    }

    FileIn& FileIn::operator>>(maths::Vector2& value)    { read(value); return *this; }
    FileIn& FileIn::operator>>(maths::Vector3& value)    { read(value); return *this; }
    FileIn& FileIn::operator>>(maths::Quaternion& value) { read(value); return *this; }
    FileIn& FileIn::operator>>(maths::Matrix34& value)   { read(value); return *this; }
    FileIn& FileIn::operator>>(maths::Matrix44& value)   { read(value); return *this; }

    bool FileIn::read(void* data, size_t size)
    {
        m_impl->file.read(reinterpret_cast<char*>(data), size);
        if ((size_t)m_impl->file.gcount() < size)
        {
            return false;
        }
        return true;
    }

    template <typename T>
    bool FileIn::read(T& value)
    {
        m_impl->file.read(reinterpret_cast<char*>(&value), sizeof(T));
        if (m_impl->file.gcount() < sizeof(T))
        {
            value = T();
            return false;
        }
        return true;
    }
}