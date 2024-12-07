#pragma once

#include <memory>

namespace file
{
    class FileOut
    {
    public:
        static FileOut from_data(const char* relative_path);
        static FileOut from_app_data(const char* relative_path);
        static FileOut from_absolute(const char* path);
        
        ~FileOut();
        FileOut(const char*);

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
        void write(const void*, int size);

    private:
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
        FileIn(const char*);

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
        void read(void*, int size);

    private:
        template<typename T>
        void read(T&);

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
}