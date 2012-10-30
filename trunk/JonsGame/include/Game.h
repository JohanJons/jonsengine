#pragma once

#include "include/Core/Engine.h"
#include "include/Input/KeyCallback.h"

namespace JonsGame
{
	class Game
	{
	public:
		Game();
		~Game();

		void Run();

        void OnKeyEvent(const JonsEngine::KeyEvent& evnt);

	private:
        JonsEngine::EngineSettings mSettings;
		JonsEngine::Engine* mEngine;
	};
}