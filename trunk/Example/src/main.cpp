#include "include/Game.h"

#include <Windows.h>

int CALLBACK WinMain(_In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine,_In_  int nCmdShow)
{
    float f = -0.1f / (0.9936f - 1.0f);
    f = (f - 0.1f) / (100.0f - 0.1f);

    JonsGame::Game* game = new JonsGame::Game();

    game->Run();

    delete game;
    
    return 0;
}
