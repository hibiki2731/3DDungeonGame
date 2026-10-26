#include "timer.h"
#include <windows.h>

unsigned int PreTime = 0;
float DeltaTime = 0.0f;
float ElapsedTime[10] = {};

void initDeltaTime() {
	PreTime = timeGetTime();
	DeltaTime = 0;
}

void setDeltaTime() {
	unsigned int curTime = timeGetTime();
	DeltaTime = (curTime - PreTime) / 1000.0f;
	DeltaTime = min(DeltaTime, 0.1f); //フレーム間の時間の最大値を0.1sに制御
	PreTime = curTime;
	
}

bool timer(int number, float interval) {
	ElapsedTime[number] += DeltaTime;
	if (ElapsedTime[number] > interval) {
		ElapsedTime[number] -= interval;
		return true;
	}
	return false;
}

void resetTimer(int number) {
	ElapsedTime[number] = 0;
}

float getDeltaTime() {
	return DeltaTime;
}
