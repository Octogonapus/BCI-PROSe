#include "velocityTBH.h"
#include "math.h"

/**
 * Initializes a velocity TBH controller
 *
 * @param tbh The TBH controller to initialize
 * @param gain Controller gain
 * @param outValApprox Approximate output at zero error for a given target velocity
 * @param ticksPerRev Sensor ticks per one revolution
 */
void vel_TBH_InitController(vel_TBH *tbh, const float gain, const int outValApprox, const float ticksPerRev)
{
	tbh->gain = gain;

	tbh->currentVelocity = 0.0;
	tbh->currentPosition = 0;
	tbh->prevPosition = 0;
	tbh->error = 0;
	tbh->prevError = 0;
	tbh->firstCross = true;
	tbh->outValApprox = outValApprox;
	tbh->outValAtZero = 0.0;

	tbh->dt = 0.0;
	tbh->prevTime = 0;

	tbh->ticksPerRev = ticksPerRev;
	tbh->targetVelocity = 0.0;

	filter_Init_DEMA(&tbh->filter);
	tbh->alpha = 0.19;
	tbh->beta = 0.0526;

	tbh->outVal = 0.0;
}

/**
 * Reinitializes a velocity TBH controller
 *
 * @param tbh The TBH controller to initialize
 */
void vel_TBH_ReInitController(vel_TBH *tbh)
{
	tbh->currentVelocity = 0.0;
	tbh->currentPosition = 0;
	tbh->prevPosition = 0;
	tbh->error = 0;
	tbh->prevError = 0;
	tbh->firstCross = true;
	tbh->outValAtZero = 0.0;
	tbh->outValChange = 0.0;

	tbh->dt = 0.0;
	tbh->prevTime = 0;

	tbh->targetVelocity = 0.0;

	tbh->outVal = 0.0;
}

/**
 * Sets new filter constants
 *
 * @param tbh The TBH controller
 * @param alpha DEMA filter constant alpha
 * @param beta DEMA filter constant beta
 */
void vel_TBH_SetFilterConstants(vel_TBH *tbh, const float alpha, const float beta)
{
	tbh->alpha = alpha;
	tbh->beta = beta;
}

/**
 * Sets the target velocity
 * This should (generally) be used when the target velocity has changed
 *
 * @param tbh The TBH controller
 * @param targetVelocity Target velocity
 * @param outValApprox Approximate output at zero error for this target velocity
 */
void vel_TBH_SetTargetVelocity(vel_TBH *tbh, const int targetVelocity, const int outValApprox)
{
	tbh->targetVelocity = targetVelocity;
	tbh->firstCross = true;

	//Set outValApprox if it is not the default value
	if (outValApprox != -1010)
	{
		tbh->outValApprox = outValApprox;
	}
}

/**
 * Gets the current error
 *
 * @param tbh The TBH controller
 */
int vel_TBH_GetError(vel_TBH *tbh)
{
	return tbh->error;
}

/**
 * Gets the current (filtered) velocity
 *
 * @param tbh The TBH controller
 */
int vel_TBH_GetVelocity(vel_TBH *tbh)
{
	return (int)tbh->currentVelocity;
}

/**
 * Gets the current target velocity
 *
 * @param tbh The TBH controller
 */
int vel_TBH_GetTargetVelocity(vel_TBH *tbh)
{
	return tbh->targetVelocity;
}

/**
 * Gets the current output
 *
 * @param tbh The TBH controller
 */
int vel_TBH_GetOutput(vel_TBH *tbh)
{
	return tbh->outVal;
}

/**
 * Sets the open-loop approximation
 *
 * @param tbh The TBH controller
 * @param outValApprox New approximate output for current targetVelocity
 */
void vel_TBH_SetOpenLoopApprox(vel_TBH *tbh, const int outValApprox)
{
	tbh->outValApprox = outValApprox;
}

/**
 * Gets the current open-loop approximation
 *
 * @param tbh The TBH controller
 */
int vel_TBH_GetOpenLoopApprox(vel_TBH *tbh)
{
	return tbh->outValApprox;
}

/**
 * Steps the controller's velocity calculation without stepping math
 *
 * @param tbh The TBH controller
 * @param sens New sensor reading
 */
int vel_TBH_StepVelocity(vel_TBH *tbh, const float sens)
{
	//Calculate timestep and scrap dt if zero
	if (util_CalculateTimestep(&(tbh->dt), &(tbh->prevTime)) == 0)
	{
		return 0;
	}

	//Calculate current velocity
	tbh->currentVelocity = (1000.0 / tbh->dt) * (sens - tbh->prevPosition) * 60.0 / tbh->ticksPerRev;
	tbh->prevPosition = sens;

	//Use a DEMA filter to smooth data
	tbh->currentVelocity = filter_DEMA(&(tbh->filter), tbh->currentVelocity, tbh->alpha, tbh->beta);

	return tbh->currentVelocity;
}

/**
 * Steps the controller calculations
 *
 * @param tbh The TBH controller
 * @param sens New sensor reading
 */
int vel_TBH_StepController(vel_TBH *tbh, const float sens)
{
	//Calculate current velocity
	vel_TBH_StepVelocity(tbh, sens);

	//Calculate error
	tbh->error = tbh->targetVelocity - tbh->currentVelocity;

	//Calculate new outVal
	//tbh->outVal = tbh->outVal + (tbh->error * tbh->gain);

	tbh->outValChange = tbh->error * tbh->gain;
	//tbh->outValChange = abs(tbh->outValChange) > TBH_OUTPUT_ERROR_ACCEL * tbh->error ? TBH_OUTPUT_ERROR_ACCEL * tbh->error : tbh->outValChange;
	tbh->outVal = tbh->outVal + tbh->outValChange;

	//Bound outVal
	tbh->outVal = tbh->outVal > 127 ? 127 : tbh->outVal;
	tbh->outVal = tbh->outVal < -127 ? -127 : tbh->outVal;

	//Check for zero crossing on error term
	if (sign(tbh->error) != sign(tbh->prevError))
	{
		//If first zero crossing since new target velocity
		if (tbh->firstCross)
		{
			//Set drive to an open loop approximation
			tbh->outVal = tbh->outValApprox;
			tbh->firstCross = false;
		}
		else
		{
			//tbh->outVal = 0.5 * (tbh->outVal + tbh->outValAtZero);
			tbh->outVal = 0.4 * (tbh->outVal + tbh->outValAtZero) + 0.2 * tbh->outVal;
		}

		//Save this outVal as the new zero base value
		tbh->outValAtZero = tbh->outVal;
	}

	//Save error
	tbh->prevError = tbh->error;

	return tbh->outVal;
}
