#pragma once

#include "include/Core/Engine.h"
#include "include/Core/EngineSettings.h"
#include "include/Window/MouseCallback.h"
#include "include/Window/KeyCallback.h"

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
        void SetupScene();
        void UpdateSun();

        JonsEngine::EngineSettings mSettings;
        JonsEngine::DebugOptions mDebugOptions;
        JonsEngine::Engine* mEngine;
        bool mRunning;
        float mSunAngle;
        const float mMoveSpeed;
    };
}