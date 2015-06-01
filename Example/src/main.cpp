#include "include/Game.h"

#include <Windows.h>
#include <functional>

int CALLBACK WinMain(_In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine,_In_  int nCmdShow)
{
    std::unique_ptr <JonsGame::Game> exampleGame(new JonsGame::Game());
    exampleGame->Run();
    
    return 0;
}
