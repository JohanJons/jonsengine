#pragma once

#include "SceneActors.h"
#include "SceneTessellation.h"

#include "Core/Engine.h"
#include "Core/EngineSettings.h"
#include "Core/Memory/HeapAligned.h"
#include "Window/MouseCallback.h"
#include "Window/KeyCallback.h"
#include "Resources/JonsPackage.h"

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

        SceneActors mGameScene;
		SceneTessellation mTessellationScene;

		bool mRunning = true;
		bool mIsDefaultScene = true;
		float mMoveSpeed = 0.1f;
    };
}