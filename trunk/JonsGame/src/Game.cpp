#include "include/Game.h"

#include "include/Core/Utils/Types.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace JonsEngine;

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

		Triangle tri;
		tri.vecA.x = -0.5; tri.vecA.y = -0.5; tri.vecA.z = 0.0;
		tri.vecB.x = 0.0; tri.vecB.y = 0.5; tri.vecB.z = 0.0;
		tri.vecC.x = 0.5; tri.vecC.y = -0.5; tri.vecC.z = 0.0;

		mEngine.GetRenderer()->DrawTriangle(tri.vecA, tri.vecB, tri.vecC);

		while (true)
		{
			mEngine.Tick();
		}

		mEngine.Destroy();
	}
}