#ifndef BANGBANG_H_
#define BANGBANG_H_

#include "filter.h"
#include "util.h"

//Bang bang controller type
typedef struct bangBang_t
{
	//Power levels
	int highPower;
	int lowPower;

	//Bangbang calculations
	float currentVelocity;
	int currentPosition;
	int prevPosition;
	int error;

	//Timestep
	unsigned int dt;
	unsigned int prevTime;

	//Input
	float ticksPerRev;
	float targetVelocity;

	//Filtering
	DEMAFilter filter;
	float alpha;
	float beta;

	//Output
	int outVal;
} bangBang;

/**
 * Initializes a bangbang controller
 *
 * @param bb The BangBang controller
 * @param highPower Output when below or at target velocity
 * @param lowPower Output when above target velocity
 * @param ticksPerRev Sensor ticks per revolution
 */
void bangBang_InitController(bangBang *bb, const int highPower, const int lowPower, const float ticksPerRev);

/**
 * Sets new filter constants
 *
 * @param bb The BangBang controller
 * @param alpha DEMA filter alpha
 * @param beta DEMA filter beta
 */
inline void bangBang_SetFilterConstants(bangBang *bb, const float alpha, const float beta);

/**
 * Sets the controller's target velocity
 *
 * @param bb The BangBang controller
 * @param targetVelocity New target velocity
 */
inline void bangBang_SetTargetVelocity(bangBang *bb, const int targetVelocity);

/**
 * Gets the current error
 *
 * @param bb The BangBang controller
 */
inline int bangBang_GetError(bangBang *bb);

/**
 * Gets the current (filtered) velocity
 *
 * @param bb The BangBang controller
 */
inline int bangBang_GetVelocity(bangBang *bb);

/**
 * Gets the current target velocity
 *
 * @param bb The BangBang controller
 */
inline float bangBang_GetTargetVelocity(bangBang *bb);

/**
 * Gets the current output
 *
 * @param bb The BangBang controller
 */
inline int bangBang_GetOutput(bangBang *bb);

/**
 * Steps the controller's velocity calculation without stepping math
 *
 * @param bb The BangBang controller
 * @param sens New sensor reading
 */
int bangBang_StepVelocity(bangBang *bb, const float sens);

/**
 * Steps the controller's calculations
 *
 * @param bb The BangBang controller
 * @param sens New sensor reading
 */
int bangBang_StepController(bangBang *bb, const float sens);

#endif
