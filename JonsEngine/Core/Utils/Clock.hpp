#pragma once

#include "Core/Utils/TimeUtils.h"

#include <chrono>

namespace JonsEngine
{
    template <typename ClockType>
    class Clock
    {
    public:
        typedef std::chrono::time_point<ClockType> TimePoint;

        Clock();
        ~Clock();

        template <typename TimeUnit>
        TimeUnit ElapsedTime(const TimePoint& sinceTimePoint) const;
        template <typename TimeUnit>
        TimeUnit ElapsedTime(const TimePoint& startTimePoint, const TimePoint& endTimePoint) const;

        TimePoint Now() const;


    private:
        ClockType mClock;
    };

    typedef Clock<std::chrono::high_resolution_clock> HiResClock;



    template <typename ClockType>
    Clock<ClockType>::Clock()
    {
    }

    template <typename ClockType>
    Clock<ClockType>::~Clock()
    {
    }


    template <typename ClockType>
    template <typename TimeUnit>
    typename TimeUnit Clock<ClockType>::ElapsedTime(const TimePoint& sinceTimePoint) const
    {
        const TimePoint currentTime = Now();

        return ElapsedTime<TimeUnit>(currentTime, sinceTimePoint);
    }

    template <typename ClockType>
    template <typename TimeUnit>
    typename TimeUnit Clock<ClockType>::ElapsedTime(const TimePoint& startTimePoint, const TimePoint& endTimePoint) const
    {
        return std::chrono::duration_cast<TimeUnit>(endTimePoint - startTimePoint);
    }


    template <typename ClockType>
    typename Clock<ClockType>::TimePoint Clock<ClockType>::Now() const
    {
        return mClock.now();
    }
}