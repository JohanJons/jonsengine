#include "include/Game.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

namespace JonsGame
{

	Game::Game()
	{
	}
		
	Game::~Game()
	{
	}

		
	void Game::Run()
	{
		mEngine.Init();

		while (true)
		{
			mEngine.Tick();
		}

		mEngine.Destroy();
	}
}