#pragma once

#include "include/Scene.h"
#include "include/SceneTessellation.h"

#include "include/Core/Engine.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Memory/HeapAligned.h"
#include "include/Window/MouseCallback.h"
#include "include/Window/KeyCallback.h"
#include "include/Resources/JonsPackage.h"

#include <functional>

namespace JonsGame
{
    class Game : public JonsEngine::HeapAligned<16>
    {
    public:
        Game();
        ~Game();

        void Run();

        void OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt);
        void OnMouseMovementEvent(const JonsEngine::MouseMovementEvent& evnt);
        void OnKeyEvent(const JonsEngine::KeyEvent& evnt);

    private:
        void SetupInputCallbacks();

        JonsEngine::EngineSettings mSettings;
        JonsEngine::Engine mEngine;
        JonsEngine::DebugOptions mDebugOptions;

        Scene mGameScene;
		SceneTessellation mTessellationScene;

		bool mRunning = true;
		bool mIsDefaultScene = true;
		float mMoveSpeed = 0.1f;
    };
}