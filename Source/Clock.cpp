#include "Clock.h"

//Function: Update
//Paramaters: -None-
//Returns: -None-
//Description: updates the fDelta varible to the number of
//             whole or part seconds since the last Update call
void Clock::Update()
{
	LARGE_INTEGER lOld = lCurrent;

	QueryPerformanceCounter(&lCurrent);

	fDelta = fCycle * (lCurrent.QuadPart - lOld.QuadPart);
}

//Function: GetClock
//Paramaters: -None-
//Returns: Pointer to persistant Clock object
//Description: Getter class to implement singleton functionaly
//             of the Clock class
Clock * Clock::GetClock()
{
	if (p_Clock == 0)
	{
		p_Clock = new Clock;

		if (p_Clock == 0)
		{
			//bad shit yo
		}
	}

	return p_Clock;
}

//Function: DestroyClock
//Paramaters: -None-
//Returns: -None-
//Description: Deletes the singleton class
void Clock::DestroyClock()
{
	if (p_Clock != 0)
	{
		delete p_Clock;
	}
}

//Function: GetDelta
//Paramaters: -None-
//Returns: Float
//Description: Getter function for the fDelta varible, representing
//             the whole or part number of seconds between two points
float Clock::GetDelta()
{
	return fDelta;
}

//Function: Clock class constructor
//Paramaters: -None-
//Returns: -None-
//Description: Querys the CPU for its clock speed and current clock count
Clock::Clock()
{
	QueryPerformanceFrequency(&lCpuRate);
	QueryPerformanceCounter(&lCurrent);

	fCycle = 1.0f / lCpuRate.QuadPart;
}

//Function: Clock class destructor
//Paramaters: -None-
//Returns: -None-
//Description: sets the internal Clock class pointer to null
Clock::~Clock()
{
	p_Clock = 0;
}

Clock* Clock::p_Clock = 0;