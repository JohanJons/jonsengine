#pragma once

#include "include/Core/Engine.h"
#include "include/Core/EngineSettings.h"
#include "include/Window/MouseCallback.h"
#include "include/Window/KeyCallback.h"
#include "include/Resources/JonsPackage.h"

#include <functional>

namespace JonsGame
{
    class JonsEngine::Scene;

    class Game
    {
    public:
        Game();
        ~Game();

        void Run();

        void OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt);
        void OnMousePositionEvent(const JonsEngine::MousePositionEvent& evnt);
        void OnKeyEvent(const JonsEngine::KeyEvent& evnt);


    private:
        void SetupInputCallbacks();
        void SetupScene();
        void SetupTestScene(JonsEngine::Scene* myScene, JonsEngine::JonsPackagePtr jonsPackage);
        void SetupSponzaScene(JonsEngine::Scene* myScene, JonsEngine::JonsPackagePtr jonsPackage);
        void UpdateSun();

        JonsEngine::EngineSettings mSettings;
        JonsEngine::DebugOptions mDebugOptions;
        std::unique_ptr<JonsEngine::Engine> mEngine;

        bool mRunning;
        float mSunAngle;
        float mMoveSpeed;

        JonsEngine::PointLightID mPointLightID;
        // TODO: refactor into IDs
        JonsEngine::Actor* mAlienActor;
    };
}