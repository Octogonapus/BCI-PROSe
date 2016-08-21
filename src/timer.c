#include "API.h"
#include "timer.h"

/**
 * Initialize a timer
 */
timer* timer_Initialize(timer *timer)
{
	timer->firstCalled = millis();
	timer->lastCalled = 0;
	timer->mark = 0;
	timer->hardMark = -1;
	timer->repeatMark = -1;

	return timer;
}

/**
 * Get time passed in ms since the last call of this function
 */
long timer_GetDT(timer *timer)
{
	long dt = millis() - timer->lastCalled;
	timer->lastCalled = millis();
	return dt;
}

/**
 * Get the time the timer was first initialized
 */
long timer_GetStartingTime(timer *timer)
{
	return timer->firstCalled;
}

/**
 * Get the time since the timer was first initialized
 */
long timer_GetDTFromStart(timer *timer)
{
	return millis() - timer->firstCalled;
}

/**
 * Place a time marker
 */
void timer_PlaceMarker(timer *timer)
{
	timer->mark = millis();
}

/**
 * Place a hard time marker
 */
void timer_PlaceHardMarker(timer *timer)
{
	if (timer->hardMark == -1)
	{
		timer->hardMark = millis();
	}
}

/**
 * Clear a hard time marker
 */
void timer_ClearHardMarker(timer *timer)
{
	timer->hardMark = -1;
}

/**
 * Get time since last time marker
 */
long timer_GetDTFromMarker(timer *timer)
{
	return millis() - timer->mark;
}

/**
 * Get time since hard time marker
 */
long timer_GetDTFromHardMarker(timer *timer)
{
	return millis() - timer->hardMark;
}

/**
 * Returns true when a time period has passed, then resets
 */
bool timer_Repeat(timer *timer, long timeMs)
{
	if (timer->repeatMark == -1)
	{
		timer->repeatMark = millis();
	}

	if (millis() - timer->repeatMark > timeMs)
	{
		timer->repeatMark = -1;
		return true;
	}

	return false;
}
