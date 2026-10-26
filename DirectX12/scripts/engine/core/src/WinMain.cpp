#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "WinMain.h"
#include "Game.h"
#include "Benchmark.h"

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT) {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//計測用: 起動引数(--bench, --stage=N など)の解析
	Bench::markStart();
	Bench::parseCommandLine();

	auto game = std::make_unique<Game>();
	game->init();
	game->runLoop();

	return game->endProcess();
}
