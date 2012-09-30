#pragma once

#include "include/Core/Engine.h"

namespace JonsGame
{
	class Game
	{
	public:
		Game();
		~Game();

		void Run();

	private:
		JonsEngine::Engine mEngine;
	};

}