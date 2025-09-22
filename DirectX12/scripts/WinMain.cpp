#include "WinMain.h"


INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT) {

	Game game;

	game.runLoop();

	return game.endProcess();
}