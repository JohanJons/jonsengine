#pragma once

#include "include/Scene/DirectionalLight.h"

#include <chrono>

namespace JonsGame
{
    class Sun
    {
    public:
        Sun(JonsEngine::DirectionalLight& dirLight);
        ~Sun();

        void Update();


        bool mIsMoving;


    private:
        typedef std::chrono::system_clock Clock;
        typedef std::chrono::time_point<Clock> TimeStamp;
        typedef std::chrono::duration<float, std::ratio<24, 1>> Intervall;

        JonsEngine::DirectionalLight& mDirLight;
        float mNightDayRatio;
        TimeStamp mTimeStart;
    };
}