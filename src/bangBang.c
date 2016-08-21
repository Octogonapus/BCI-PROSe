#include "API.h"
#include "bangBang.h"
#include "math.h"

/**
 * Initializes a bangbang controller
 *
 * @param bb The BangBang controller
 * @param highPower Output when below or at target velocity
 * @param lowPower Output when above target velocity
 * @param ticksPerRev Sensor ticks per revolution
 */
void bangBang_InitController(bangBang *bb, const int highPower, const int lowPower, const float ticksPerRev)
{
	bb->highPower = highPower;
	bb->lowPower = lowPower;

	bb->currentVelocity = 0.0;
	bb->currentPosition = 0;
	bb->prevPosition = 0;
	bb->error = 0;

	bb->dt = 0.0;
	bb->prevTime = 0;

	bb->ticksPerRev = ticksPerRev;
	bb->targetVelocity = 0.0;

	filter_Init_DEMA(&bb->filter);
	bb->alpha = 0.19;
	bb->beta = 0.0526;

	bb->outVal = 0;
}

/**
 * Sets new filter constants
 *
 * @param bb The BangBang controller
 * @param alpha DEMA filter alpha
 * @param beta DEMA filter beta
 */
inline void bangBang_SetFilterConstants(bangBang *bb, const float alpha, const float beta)
{
	bb->alpha = alpha;
	bb->beta = beta;
}

/**
 * Sets the controller's target velocity
 *
 * @param bb The BangBang controller
 * @param targetVelocity New target velocity
 */
inline void bangBang_SetTargetVelocity(bangBang *bb, const int targetVelocity)
{
	bb->targetVelocity = targetVelocity;
}

/**
 * Gets the current error
 *
 * @param bb The BangBang controller
 */
inline int bangBang_GetError(bangBang *bb)
{
	return bb->error;
}

/**
 * Gets the current (filtered) velocity
 *
 * @param bb The BangBang controller
 */
inline int bangBang_GetVelocity(bangBang *bb)
{
	return (int)bb->currentVelocity;
}

/**
 * Gets the current target velocity
 *
 * @param bb The BangBang controller
 */
inline float bangBang_GetTargetVelocity(bangBang *bb)
{
	return bb->targetVelocity;
}

/**
 * Gets the current output
 *
 * @param bb The BangBang controller
 */
inline int bangBang_GetOutput(bangBang *bb)
{
	return bb->outVal;
}

/**
 * Steps the controller's velocity calculation without stepping math
 *
 * @param bb The BangBang controller
 * @param sens New sensor reading
 */
int bangBang_StepVelocity(bangBang *bb, const float sens)
{
	//Calculate timestep and scrap if zero
	if (util_CalculateTimestep(&(bb->dt), &(bb->prevTime)) == 0)
	{
		return 0;
	}

	//Calculate current velocity
	bb->currentVelocity = (1000.0 / bb->dt) * (sens - bb->prevPosition) * 60.0 / bb->ticksPerRev;
	bb->prevPosition = sens;

	//Use a DEMA filter to smooth velocity
	bb->currentVelocity = filter_DEMA(&(bb->filter), bb->currentVelocity, bb->alpha, bb->beta);

	return bb->currentVelocity;
}

/**
 * Steps the controller's calculations
 *
 * @param bb The BangBang controller
 * @param sens New sensor reading
 */
int bangBang_StepController(bangBang *bb, const float sens)
{
	//Calculate current velocity and scrap dt if zero
	if (bangBang_StepVelocity(bb, sens) == 0)
	{
		return 0;
	}

	//Calculate error
	bb->error = bb->targetVelocity - bb->currentVelocity;

	//Calculate new outVal
	//Low power when above target velocity
	if (bb->currentVelocity > bb->targetVelocity)
	{
		bb->outVal = bb->lowPower;
	}
	//High power when below or equal to target velocity
	else if (bb->currentVelocity <= bb->targetVelocity)
	{
		bb->outVal = bb->highPower;
	}

	return bb->outVal;
}
