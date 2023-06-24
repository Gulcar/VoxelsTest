#pragma once

#include <chrono>
#include <iostream>

#define TIME_FUNCTION(name) \
    FuncTimer timer(name)

class FuncTimer
{
    using Clock = std::chrono::high_resolution_clock;

public:
    FuncTimer(const char* name)
    {
        m_name = name;
        m_startTime = Clock::now();
    }

    ~FuncTimer()
    {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - m_startTime);
        float ftime = duration.count() / 1000.0f;
        std::cout << m_name << ": " << ftime << "ms\n";
    }

private:
    const char* m_name;
    Clock::time_point m_startTime;
};
