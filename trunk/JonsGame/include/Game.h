#pragma once

#include "include/Core/Engine.h"
#include "include/Input/MouseCallback.h"
#include "include/Input/KeyCallback.h"
#include "include/Video/OpenGL/UniformBuffer.h"

namespace JonsGame
{
    class Game
    {
    public:
        Game();
        ~Game();

        void Run();

        void OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt);
        void OnMouseMotionEvent(const JonsEngine::MouseMotionEvent& evnt);
        void OnKeyEvent(const JonsEngine::KeyEvent& evnt);


    private:
        void SetupInputCallbacks();
        void TestRendering();

        JonsEngine::EngineSettings mSettings;
        JonsEngine::Engine* mEngine;
        bool mRunning;

        JonsEngine::UniformBuffer mUniBuffer;
    };
}