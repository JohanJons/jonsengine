#pragma once

#include "include/Core/Engine.h"
#include "include/Input/MouseCallback.h"
#include "include/Input/KeyCallback.h"

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
        JonsEngine::EngineSettings mSettings;
		JonsEngine::Engine* mEngine;
	};
}