#pragma once

#include <chrono>

namespace JonsEngine
{
    template <typename ClockType>
    class Timer
    {
    public:
        typedef std::chrono::time_point<ClockType> TimePoint;

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
        template <typename TimeUnit>
        TimeUnit ElapsedTime(const TimePoint& sinceTimePoint) const;
        template <typename TimeUnit>
        TimeUnit ElapsedTime(const TimePoint& startTimePoint, const TimePoint& endTimePoint) const;
        TimePoint Now() const;

        bool IsRunning() const;


    private:
        ClockType mClock;
        TimePoint mStartTime;
        TimePoint mStopTime;
        bool mIsRunning;
    };

    typedef Timer<std::chrono::high_resolution_clock> HiResTimer;



    template <typename ClockType>
    Timer<ClockType>::Timer() : mIsRunning(false)
    {
    }

    template <typename ClockType>
    Timer<ClockType>::~Timer()
    {
    }


    template <typename ClockType>
    void Timer<ClockType>::Start()
    {
        mStartTime = mClock.now();
        mIsRunning = true;
    }

    template <typename ClockType>
    void Timer<ClockType>::Stop()
    {
        mStopTime = mClock.now();
        mIsRunning = false;
    }


    template <typename ClockType>
    template <typename TimeUnit>
    typename TimeUnit Timer<ClockType>::ElapsedTime() const
    {
        if (mIsRunning)
            return ElapsedTime<TimeUnit>(mStartTime);
        else
            return ElapsedTime<TimeUnit>(mStartTime, mStopTime);
    }

    template <typename ClockType>
    template <typename TimeUnit>
    typename TimeUnit Timer<ClockType>::ElapsedTime(const TimePoint& sinceTimePoint) const
    {
        const TimePoint currentTime = Now();

        return ElapsedTime<TimeUnit>(currentTime, sinceTimePoint);
    }

    template <typename ClockType>
    template <typename TimeUnit>
    typename TimeUnit Timer<ClockType>::ElapsedTime(const TimePoint& startTimePoint, const TimePoint& endTimePoint) const
    {
        return std::chrono::duration_cast<TimeUnit>(endTimePoint - startTimePoint);
    }

    template <typename ClockType>
    typename Timer<ClockType>::TimePoint Timer<ClockType>::Now() const
    {
        return mClock.now();
    }


    template <typename ClockType>
    bool Timer<ClockType>::IsRunning() const
    {
        return mIsRunning;
    }
}