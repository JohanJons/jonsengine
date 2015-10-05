#pragma once

#include <chrono>

namespace JonsEngine
{
    template <typename ClockType>
    class Timer
    {
    public:
        typedef std::chrono::microseconds Microseconds;
        typedef std::chrono::milliseconds Milliseconds;
        typedef std::chrono::seconds Seconds;
        typedef std::chrono::minutes Minutes;
        typedef std::chrono::hours Hours;

        Timer();
        ~Timer();

        void Start();
        void Stop();

        template <typename TimeUnit>
        TimeUnit ElapsedTime() const;


    private:
        ClockType mClock;
    };

    typedef Timer<std::chrono::high_resolution_clock> HiResTimer;
}