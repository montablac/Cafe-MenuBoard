#pragma once

#ifndef Windows
#include <Windows.h>
#endif // !Windows

/*

Clock class

Designed to help keep time by measuring CPU cycles and reporting back
a value in part or whole seconds

usefull to ensure things move in a smooth manner no matter the CPU load
of the computer its running on

Created by: Joshua Print

not an original idea, but the code is my own

*/

class Clock
{
public:
	void Update();

	static Clock* GetClock();

	static void DestroyClock();

	float GetDelta();
private:
	LARGE_INTEGER lCpuRate;
	LARGE_INTEGER lCurrent;
	float fDelta = 0.0f;
	float fCycle = 0.0f;
	static Clock* p_Clock;

	Clock();

	~Clock();
};