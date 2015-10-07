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
        TimePoint Now() const;

        bool IsRunning() const;


    private:
        ClockType mClock;
        TimePoint mStartTime;
        TimePoint mStopTime;
        bool mRunning;
    };

    typedef Timer<std::chrono::high_resolution_clock> HiResTimer;



    template <typename ClockType>
    Timer<ClockType>::Timer() : mRunning(false)
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
        {
            const TimePoint currentTime = mClock.now();

            return currentTime - mStartTime;
        }
        else
        {
            return mStopTime - mStartTime;
        }
    }

    template <typename ClockType>
    template <typename TimeUnit>
    typename TimeUnit Timer<ClockType>::ElapsedTime(const TimePoint& sinceTimePoint) const
    {
        return TimeUnit();
    }

    template <typename ClockType>
    typename Timer<ClockType>::TimePoint Timer<ClockType>::Now() const
    {
        return TimePoint();
    }


    template <typename ClockType>
    bool Timer<ClockType>::IsRunning() const
    {
        return mIsRunning;
    }
}