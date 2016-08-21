#include "API.h"
#include "velocityPID.h"
#include "math.h"

/**
 * Initializes a velocity PID controller
 *
 * @param pid The PID controller
 * @param kP Proportional gain
 * @param kD Derivative gain
 * @param ticksPerRev Sensor ticks per one revolution
 */
void vel_PID_InitController(vel_PID *pid, const float kP, const float kD, const float ticksPerRev)
{
	pid->kP = kP;
	pid->kD = kD;

	pid->error = 0;
	pid->currentVelocity = 0.0;
	pid->prevError = 0;
	pid->prevPosition = 0;
	pid->prevTime = 0;
	pid->dt = 0.0;
	pid->derivative = 0;

	pid->ticksPerRev = ticksPerRev;
	pid->targetVelocity = 0.0;

	filter_Init_DEMA(&pid->filter);
	pid->alpha = 0.19;
	pid->beta = 0.0526;

	pid->outVal = 0.0;
}

/**
 * Sets new filter constants
 *
 * @param pid The PID controller
 * @param alpha DEMA alpha gain
 * @param beta DEMA beta gain
 */
void vel_PID_SetFilterConstants(vel_PID *pid, const float alpha, const float beta)
{
	pid->alpha = alpha;
	pid->beta = beta;
}

/**
 * Sets the controller's target velocity
 *
 * @param pid The PID controller
 * @param targetVelocity New target velocity
 */
void vel_PID_SetTargetVelocity(vel_PID *pid, const int targetVelocity)
{
	pid->targetVelocity = targetVelocity;
}

/**
 * Gets the current error
 *
 * @param pid The PID controller
 */
int vel_PID_GetError(vel_PID *pid)
{
	return pid->error;
}

/**
 * Gets the current (filtered) velocity
 *
 * @param pid The PID controller
 */
float vel_PID_GetVelocity(vel_PID *pid)
{
	return pid->currentVelocity;
}

/**
 * Gets the current (filtered) velocity
 *
 * @param pid The PID controller
 */
float vel_PID_GetTargetVelocity(vel_PID *pid)
{
	return pid->targetVelocity;
}

/**
 * Gets the current output
 *
 * @param pid The PID controller
 */
int vel_PID_GetOutput(vel_PID *pid)
{
	return pid->outVal;
}

/**
 * Steps the controller's velocity calculation without stepping math
 *
 * @param pid The PID controller
 * @param sens New sensor reading
 */
int vel_PID_StepVelocity(vel_PID *pid, const float sens)
{
	//Calculate timestep and scrap if zero
	if (util_CalculateTimestep(&(pid->dt), &(pid->prevTime)) == 0)
	{
		return 0;
	}

	//Calculate current velocity
	pid->currentVelocity = (1000.0 / pid->dt) * (sens - pid->prevPosition) * 60.0 / pid->ticksPerRev;
	pid->prevPosition = sens;

	//Use a DEMA filter to smooth velocity
	pid->currentVelocity = filter_DEMA(&(pid->filter), pid->currentVelocity, pid->alpha, pid->beta);

	return pid->currentVelocity;
}

/**
 * Steps the controller's calculations
 *
 * @param pid The PID controller
 * @param sens New sensor reading
 */
int vel_PID_StepController(vel_PID *pid, const float sens)
{
	//Calculate current velocity and scrap dt if zero
	if (vel_PID_StepVelocity(pid, sens) == 0)
	{
		return 0;
	}

	//Calculate error
	pid->error = pid->targetVelocity - pid->currentVelocity;

	//Calculate derivative
	pid->derivative = (pid->error - pid->prevError) / pid->dt;
	pid->prevError = pid->error;

	//Sum outVal to compute change in output instead out output itself
	pid->outVal += (pid->error * pid->kP) + (pid->derivative * pid->kD);

	return pid->outVal;
}
