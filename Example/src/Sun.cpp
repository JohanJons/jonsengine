#include "include/Sun.h"

using namespace JonsEngine;

namespace JonsGame
{
    Sun::Sun(JonsEngine::DirectionalLight& dirLight) : mDirLight(dirLight), mIsMoving(true), mSpeed()
    {
    }

    Sun::~Sun()
    {
    }


    void Sun::Update()
    {
        if (!mIsMoving)
            return;

        mDirLight.mLightDirection = Vec3(glm::cos(angleDegreesTime), glm::sin(angleDegreesTime), 0.0f);
    }
}