#include "GameCore.h"
#include "Game.h"
#include <Windows.h>

extern void f();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	f();
	Game game;
	return game.Start();
}
