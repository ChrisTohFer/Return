#include "file_system.h"

#include <assert.h>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace file
{
    template<size_t buf_size>
    void get_data_path(const char* relative_path, char (&path_out)[buf_size])
    {
        snprintf(path_out, buf_size, "%s/data/%s", std::filesystem::current_path().c_str(), relative_path);
    }
    template<size_t buf_size>
    void get_appdata_path(const char* relative_path, char (&path_out)[buf_size])
    {
        assert(!"get_appdata_path() not implemented yet!");
    }

    //FileOut =========================================================================

    struct FileOut::Impl
    {
        std::ofstream file;
    };
    FileOut FileOut::from_data(const char *relative_path)
    {
        char path[1024];
        get_data_path(relative_path, path);
        return FileOut(path);
    }
    FileOut FileOut::from_app_data(const char *relative_path)
    {
        char path[1024];
        get_appdata_path(relative_path, path);
        return FileOut(path);
    }
    FileOut FileOut::from_absolute(const char *path)
    {
        return FileOut(path);
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
        int size = value.size();
        write(size);
        write(value.data(), size);
        return *this;
    }
    FileOut& FileOut::operator<<(const char* value)
    {
        int size = strlen(value);
        m_impl->file.write(reinterpret_cast<const char*>(&size), sizeof(int));
        m_impl->file.write(value, size);
        return *this;
    }

    void FileOut::write(const void* data, int size)
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
        char path[1024];
        get_data_path(relative_path, path);
        return FileIn(path);
    }
    FileIn FileIn::from_app_data(const char *relative_path)
    {
        char path[1024];
        get_appdata_path(relative_path, path);
        return FileIn(path);
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
        int size;
        read(size);
        value.resize(size);
        read(value.data(), size);
        return *this;
    }

    void FileIn::read(void* data, int size)
    {
        m_impl->file.read(reinterpret_cast<char*>(data), size);
    }

    template <typename T>
    void FileIn::read(T& value)
    {
        m_impl->file.read(reinterpret_cast<char*>(&value), sizeof(T));
    }
}