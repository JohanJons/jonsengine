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
        if (!IsMoving())
            return;

        const auto now = Clock::now();
        const auto timeDiff = now - mTimeStart;
        const float count = glm::clamp(0.2f + std::chrono::duration_cast<Intervall>(timeDiff).count(), 0.2f, 1.0f);

        assert(mNightDayRatio <= 1.0f);
        //if (count >= 0.5f + 0.5f * mNightDayRatio)
        //    mTimeStart = now;

        const float pi = glm::pi<float>();
        const float latitude = 40 * pi / 180;

        const double julian = 2451604.875;

        const float hoursSinceMidnight = count * 24;
        const double solarTime = pi / 12 * (hoursSinceMidnight
            + 0.17 * glm::sin(4 * pi * (julian - 80) / 373)
            - 0.129 * glm::sin(2 * pi * (julian - 8) / 355));
        const double solarDeclination = 0.4093 * glm::sin(2 * pi * (julian - 81) / 368);

        const float solarZenith = static_cast<float>(glm::acos(glm::sin(latitude) * glm::sin(solarDeclination) + glm::cos(latitude) * glm::cos(solarDeclination) * glm::cos(solarTime)));
        //const float lightZenith = glm::min(solarZenith, pi / 2 - 0.2f);   no clamping for day/night cycle?
        const float solarAzimuth = static_cast<float>(solarTime);

        mDirLight.SetLightDirection(Vec3(glm::sin(solarAzimuth) * glm::sin(solarZenith), glm::cos(solarZenith), glm::cos(solarAzimuth) * glm::sin(solarZenith)));
    }


    void Sun::SetIsMoving(const bool moving)
    {
        mIsMoving = moving;
        if (moving)
            mTimeStart = Clock::now();
    }

    bool Sun::IsMoving() const
    {
        return mIsMoving;
    }
}