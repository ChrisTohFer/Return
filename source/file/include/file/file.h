#pragma once

#include "maths/maths_forward.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#define DEFINE_ENUM_SERIALIZE_FUNCTIONS(type)                  \
inline ::file::FileOut& operator<<(::file::FileOut& f, type v) \
{                                                              \
    return f << (uint64_t)v;                                   \
}                                                              \
inline ::file::FileIn& operator>>(::file::FileIn& f, type& v)  \
{                                                              \
    uint64_t v_int;                                            \
    f >> v_int;                                                \
    v = (type)v_int;                                           \
    return f;                                                  \
}

#define DEFINE_SERIALIZATION_FUNCTIONS(...)                            \
    void write(::file::FileOut& f) const { f.write_all(__VA_ARGS__); } \
    void read(::file::FileIn& f) { f.read_all(__VA_ARGS__); };

namespace file
{
    std::filesystem::path get_data_path(const char* relative_path);
    std::filesystem::path get_appdata_path(const char* relative_path);

    std::optional<std::string> read_string_from_data(const char* relative_path);
    std::optional<std::string> read_string_from_appdata(const char* relative_path);
    std::optional<std::string> read_string_from_absolute(const char* path);

    bool write_string_to_data(const char* relative_path, const char* string);
    bool write_string_to_appdata(const char* relative_path, const char* string);
    bool write_string_to_absolute(const char* path, const char* string);

    class FileOut
    {
    public:
        static FileOut from_data(const char* relative_path);
        static FileOut from_app_data(const char* relative_path);
        static FileOut from_absolute(const char* path);
        
        ~FileOut();

        bool valid() const;

        FileOut& operator<<(const int8_t&);
        FileOut& operator<<(const int16_t&);
        FileOut& operator<<(const int32_t&);
        FileOut& operator<<(const int64_t&);
        FileOut& operator<<(const uint8_t&);
        FileOut& operator<<(const uint16_t&);
        FileOut& operator<<(const uint32_t&);
        FileOut& operator<<(const uint64_t&);
        FileOut& operator<<(const float&);
        FileOut& operator<<(const double&);
        FileOut& operator<<(const bool&);
        FileOut& operator<<(const std::string&);
        FileOut& operator<<(const char*);

        FileOut& operator<<(const maths::Vector2&);
        FileOut& operator<<(const maths::Vector3&);
        FileOut& operator<<(const maths::Quaternion&);
        FileOut& operator<<(const maths::Matrix34&);
        FileOut& operator<<(const maths::Matrix44&);

        template<typename ElementT>
        FileOut& operator<<(const ElementT&);
        template<typename ElementT>
        FileOut& operator<<(const std::vector<ElementT>&);
        template<typename ... ElementT>
        FileOut& write_all(const ElementT&...);
        void write(const void*, size_t size);

    private:
        FileOut(const char*);

        template<typename T>
        void write(const T&);

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };

    class FileIn
    {
    public:
        static FileIn from_data(const char* relative_path);
        static FileIn from_app_data(const char* relative_path);
        static FileIn from_absolute(const char* path);

        ~FileIn();

        bool valid() const;
        
        FileIn& operator>>(int8_t&);
        FileIn& operator>>(int16_t&);
        FileIn& operator>>(int32_t&);
        FileIn& operator>>(int64_t&);
        FileIn& operator>>(uint8_t&);
        FileIn& operator>>(uint16_t&);
        FileIn& operator>>(uint32_t&);
        FileIn& operator>>(uint64_t&);
        FileIn& operator>>(float&);
        FileIn& operator>>(double&);
        FileIn& operator>>(bool&);
        FileIn& operator>>(std::string&);

        FileIn& operator>>(maths::Vector2&);
        FileIn& operator>>(maths::Vector3&);
        FileIn& operator>>(maths::Quaternion&);
        FileIn& operator>>(maths::Matrix34&);
        FileIn& operator>>(maths::Matrix44&);

        template<typename ElementT>
        FileIn& operator>>(ElementT&);
        template<typename ElementT>
        FileIn& operator>>(std::vector<ElementT>&);
        template<typename ... ElementT>
        FileIn& read_all(ElementT&...);
        //returns true if read successfully, false if hit end of file
        bool read(void*, size_t size);

    private:
        FileIn(const char*);

        template<typename T>
        bool read(T&);

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };

    //inline defs

    template<typename ElementT>
    FileOut& FileOut::operator<<(const ElementT& element)
    {
        //default to write function if we didn't define <<
        //if running into an error here you need to define << or write on ElementT
        element.write(*this);
        return *this;
    }
    template<typename ElementT>
    FileOut& FileOut::operator<<(const std::vector<ElementT>& vec)
    {
        *this << (int)vec.size();
        for(auto& elem : vec)
        {
            *this << elem;
        }
        return *this;
    }
    template<typename ... ElementT>
    FileOut& FileOut::write_all(const ElementT&... elements)
    {
        ((*this << elements), ...);
        return *this;
    }

    template<typename ElementT>
    FileIn& FileIn::operator>>(ElementT& element)
    {
        //default to read function if we didn't define >>
        //if running into an error here you need to define >> or read on ElementT
        element.read(*this);
        return *this;
    }
    template<typename ElementT>
    FileIn& FileIn::operator>>(std::vector<ElementT>& vec)
    {
        int size;
        *this >> size;
        vec.resize(size);
        for(auto& elem : vec)
        {
            *this >> elem;
        }
        return *this;
    }
    template<typename ... ElementT>
    FileIn& FileIn::read_all(ElementT&... elements)
    {
        ((*this >> elements), ...);
        return *this;
    }
}