#pragma once

#include "SceneActors.h"
#include "SceneTessellation.h"

#include "Engine.h"
#include "EngineSettings.h"
#include "HeapAligned.h"
#include "MouseCallback.h"
#include "KeyCallback.h"
#include "JonsPackage.h"

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