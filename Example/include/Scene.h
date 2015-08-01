#pragma once

#include "include/Core/Engine.h"

#include <string>

namespace JonsGame
{
    class Scene
    {
    public:
        Scene(JonsEngine::Scene& jonsScene, JonsEngine::ResourceManifest& resManifest, const std::string& jonsPkgName);
        ~Scene();

        JonsEngine::Scene& GetJonsScene();
        JonsEngine::PointLight& GetMovingPointLight();
        JonsEngine::DirectionalLight& GetSunDirLight();
        JonsEngine::Actor& GetAnimatedWuson();


    private:
        JonsEngine::Scene& mScene;
        JonsEngine::PointLightID mMovingPointLightID;
        JonsEngine::DirectionalLightID mSunDirLightID;
        JonsEngine::Actor* mWuson;
    };
}