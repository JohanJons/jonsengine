#include "include/Sun.h"

using namespace JonsEngine;

namespace JonsGame
{
    Sun::Sun(JonsEngine::DirectionalLight& dirLight) : mDirLight(dirLight), mIsMoving(true), mNightDayRatio(0.5f), mSpeed(0.0f), mTimeStart(Clock::now())
    {
    }

    Sun::~Sun()
    {
    }


    void Sun::Update()
    {
        if (!mIsMoving)
            return;

        const auto now = Clock::now();
        const auto timeDiff = now - mTimeStart;
        const float count = std::chrono::duration_cast<Intervall>(timeDiff).count();
        if (count >= 0.5f)
            mTimeStart = now;

        const float angle = count * glm::two_pi<float>() - glm::pi<float>();

        //const uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 360;
        //const float angleDegreesTime = glm::radians(static_cast<float>(time));

        mDirLight.mLightDirection = Vec3(glm::cos(angle), glm::sin(angle), -0.6f);
    }
}