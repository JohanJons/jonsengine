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

        /*const auto now = Clock::now();
        const auto timeDiff = now - mTimeStart;
        const float count = std::chrono::duration_cast<Intervall>(timeDiff).count();

        assert(mNightDayRatio <= 1.0f);
        if (count >= 0.5f + 0.5f * mNightDayRatio)
            mTimeStart = now;

        const float angle = count * glm::two_pi<float>() - glm::pi<float>();*/
        const float pi = glm::pi<float>();
        const float latitude = 40 * pi / 180;
        const float solarDeclination = 0.0f;
        const float solarTime = 0.5f;   // TODO: variable

        const float solarZenith = glm::acos(glm::sin(latitude) * glm::sin(solarDeclination) + glm::cos(latitude) * glm::cos(solarDeclination) * glm::cos(solarTime));
        const float lightZenith = glm::min(solarZenith, pi / 2 - 0.2f);
        const float solarAzimuth = solarTime;

        mDirLight.mLightDirection = Vec3(glm::sin(solarAzimuth) * glm::sin(lightZenith), glm::cos(lightZenith), glm::cos(solarAzimuth) * glm::sin(lightZenith));
    }
}