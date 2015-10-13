#pragma once

#include "include/Core/Utils/Time.h"
#include "include/Core/Utils/Clock.hpp"

#include <chrono>

namespace JonsEngine
{
    class Timer
    {
    public:
        Timer();
        ~Timer();

        void Start();
        void Stop();

        bool IsRunning() const;

        template <typename TimeUnit>
        TimeUnit ElapsedTime() const;


    private:
        HiResClock mClock;
        HiResClock::TimePoint mStartTime;
        HiResClock::TimePoint mStopTime;
        bool mIsRunning;
    };



    template <typename TimeUnit>
    typename TimeUnit Timer::ElapsedTime() const
    {
        if (mIsRunning)
            return mClock.ElapsedTime<TimeUnit>(mStartTime);
        else
            return mClock.ElapsedTime<TimeUnit>(mStartTime, mStopTime);
    }
}