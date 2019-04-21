#include "Core/Utils/Timer.h"

namespace JonsEngine
{
    Timer::Timer() : mIsRunning(false)
    {
    }

    Timer::~Timer()
    {
    }


    void Timer::Start()
    {
        mStartTime = mClock.Now();
        mIsRunning = true;
    }

    void Timer::Stop()
    {
        mStopTime = mClock.Now();
        mIsRunning = false;
    }


    bool Timer::IsRunning() const
    {
        return mIsRunning;
    }
}