#include <Windows.h>
#include "include/Editor.h"

using namespace JonsEditor;

int CALLBACK WinMain(_In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine,_In_  int nCmdShow)
{
	Editor editor;
	editor.Run();

    return 0;
}
