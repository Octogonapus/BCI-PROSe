#ifndef TIMER_H_
#define TIMER_H_

#include <stdbool.h>

//Timer representation
typedef struct timer_t
{
	long firstCalled; //Time the timer was initialized
	long lastCalled;  //Last time the timer was called
	long mark;        //Current time mark
	long hardMark;    //Current hard type time mark
	long repeatMark;  //Hard type mark used for repeat function
} timer;

/**
 * Initialize a timer
 *
 * @param timer The timer
 */
timer* timer_Initialize(timer *timer);

/**
 * Get time passed in ms since the last call of this function
 *
 * @param timer The timer
 */
long timer_GetDT(timer *timer);

/**
 * Get the time the timer was first initialized
 *
 * @param timer The timer
 */
long timer_GetStartingTime(timer *timer);

/**
 * Get the time since the timer was first initialized
 *
 * @param timer The timer
 */
inline long timer_GetDTFromStart(timer *timer);

/**
 * Place a time marker
 *
 * @param timer The timer
 */
inline void timer_PlaceMarker(timer *timer);

/**
 * Place a hard time marker
 *
 * @param timer The timer
 */
inline void timer_PlaceHardMarker(timer *timer);

/**
 * Clear a hard time marker
 *
 * @param timer The timer
 */
inline void timer_ClearHardMarker(timer *timer);

/**
 * Get time since last time marker
 *
 * @param timer The timer
 */
inline long timer_GetDTFromMarker(timer *timer);

/**
 * Get time since hard time marker
 *
 * @param timer The timer
 */
inline long timer_GetDTFromHardMarker(timer *timer);

/**
 * Returns true when a time period has passed, then resets
 *
 * @param timer The timer
 * @param timeMs Repeat time in ms
 */
bool timer_Repeat(timer *timer, long timeMs);

#endif
