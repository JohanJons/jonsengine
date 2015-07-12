#pragma once

#include "include/Scene/DirectionalLight.h"

namespace JonsGame
{
    class Sun
    {
    public:
        Sun(JonsEngine::DirectionalLight& dirLight);
        ~Sun();

        void Update();


    private:
        JonsEngine::DirectionalLight& mSun;

        float mSunSpeed;
        bool mSunMoving;
    };
}