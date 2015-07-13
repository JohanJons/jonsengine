#pragma once

#include "include/Scene.h"
#include "include/Sun.h"

#include "include/Core/Engine.h"
#include "include/Core/EngineSettings.h"
#include "include/Window/MouseCallback.h"
#include "include/Window/KeyCallback.h"
#include "include/Resources/JonsPackage.h"

#include <functional>

namespace JonsGame
{
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


        JonsEngine::EngineSettings mSettings;
        JonsEngine::Engine mEngine;
        JonsEngine::DebugOptions mDebugOptions;

        Scene mGameScene;
        Sun mSun;

        bool mRunning;
        float mMoveSpeed;
    };
}