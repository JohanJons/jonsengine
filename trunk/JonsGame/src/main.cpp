#include "include/Game.h"

#include <Windows.h>

int CALLBACK WinMain(_In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine,_In_  int nCmdShow)
{
	JonsGame::Game* game = new JonsGame::Game();

	game->Run();

	delete game;

	return 0;
}
