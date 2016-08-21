#include "positionPID.h"
#include "util.h"
#include "API.h"
#include "math.h"

/**
 * Initializes a position PID controller
 *
 * @param pid The PID controller to initialize
 * @param kP Proportional gain
 * @param kI Integral gain
 * @param kD Derivative gain
 */
void pos_PID_InitController(pos_PID *pid, const float kP, const float kI, const float kD)
{
	pid->kP = kP;
	pid->kI = kI;
	pid->kD = kD;
	pid->kBias = 0.0;

	pid->error = 0;
	pid->prevError = 0;
	pid->integral = 0;
	pid->derivative = 0;

	pid->errorThreshold = 0;
	pid->integralLimit = 1000000;

	pid->dt = 0;
	pid->prevTime = 0;

	pid->targetPos = 0;

	pid->outVal = 0;
}

/**
 * Initializes a position PID controller
 *
 * @param pid The PID controller to initialize
 * @param kP Proportional gain
 * @param kI Integral gain
 * @param kD Derivative gain
 * @param kBias Controller bias added to final output
 * @param errorThreshold Minimum error needed to sum for integral term
 * @param integralLimit Maximum integral term value
 */
void pos_PID_InitController_Full(pos_PID *pid, const float kP, const float kI, const float kD, const float kBias, const int errorThreshold, const int integralLimit)
{
	pid->kP = kP;
	pid->kI = kI;
	pid->kD = kD;
	pid->kBias = kBias;

	pid->error = 0;
	pid->prevError = 0;
	pid->integral = 0;
	pid->derivative = 0;

	pid->errorThreshold = errorThreshold;
	pid->integralLimit = integralLimit;

	pid->dt = 0;
	pid->prevTime = 0;

	pid->targetPos = 0;

	pid->outVal = 0;
}

/**
 * Sets the target position
 *
 * @param pid The PID controller
 * @param targetPos Target position
 */
void pos_PID_SetTargetPosition(pos_PID *pid, const int targetPos)
{
	pid->targetPos = targetPos;
}

/**
 * Gets the current error
 *
 * @param pid The
 */
int pos_PID_GetError(pos_PID *pid)
{
	return pid->error;
}

/**
 * Gets the current output
 *
 * @param pid The PID controller
 */
int pos_PID_GetOutput(pos_PID *pid)
{
	return pid->outVal;
}

/**
 * Steps the controller's calculations
 *
 * @param pid The pid controller
 * @param sens New sensor reading
 */
int pos_PID_StepController(pos_PID *pid, const float sens)
{
	//Calculate timestep and scrap if zero
	if (util_CalculateTimestep(&(pid->dt), &(pid->prevTime)) == 0)
	{
		return 0;
	}

	//Calculate error
	pid->error = pid->targetPos - sens;

	//If error is higher than errorThreshold and integral is less than integralLimit, sum
	if (abs(pid->error) > pid->errorThreshold && abs(pid->integral) < pid->integralLimit)
	{
		pid->integral = pid->integral + (pid->error * pid->dt);

		//Reset integral if controller reached targetPos or overshot
		if (pid->error == 0 || sign(pid->error) != sign(pid->prevError))
		{
			pid->integral = 0;
		}
		//Bound integral
		else
		{
			pid->integral = pid->integral * pid->kI > 127 ? 127.0 / pid->kI : pid->integral;
			pid->integral = pid->integral * pid->kI < -127 ? -127.0 / pid->kI : pid->integral;
		}
	}

	///Calculate derivative
	pid->derivative = (pid->error - pid->prevError) / pid->dt;
	pid->prevError = pid->error;

	//Calculate output
	pid->outVal = (pid->error * pid->kP) + (pid->integral * pid->kI) + (pid->derivative * pid->kD) + pid->kBias;

	return pid->outVal;
}
