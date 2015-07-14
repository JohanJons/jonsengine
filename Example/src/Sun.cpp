#include "include/Sun.h"

using namespace JonsEngine;

namespace JonsGame
{
    Sun::Sun(JonsEngine::DirectionalLight& dirLight) : mDirLight(dirLight), mIsMoving(true), mNightDayRatio(0.5f), mTimeStart(Clock::now())
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

        assert(mNightDayRatio <= 1.0f);
        if (count >= 0.5f + 0.5f * mNightDayRatio)
            mTimeStart = now;

        const float angle = count * glm::two_pi<float>() - glm::pi<float>();

        mDirLight.mLightDirection = Vec3(glm::cos(angle), glm::sin(angle), glm::sin(angle));
    }
}