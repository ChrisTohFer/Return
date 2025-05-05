#pragma once

#include <chrono>

namespace re
{
    class Timer
    {
    public:
        Timer()
        {
            m_start = std::chrono::system_clock::now();
        }

        float age_seconds()
        {
            using namespace std::chrono;
            return 1e-9f * duration_cast<nanoseconds>(system_clock::now() - m_start).count();
        }

    private:
        std::chrono::system_clock::time_point m_start;
    };
}