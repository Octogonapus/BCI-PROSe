#ifndef VELOCITYPID_H_
#define VELOCITYPID_H_

#include <stdbool.h>
#include "filter.h"
#include "util.h"

//A velocity PID controller
typedef struct vel_PID_t
{
  //PID constants
	float kP;
	float kD;

	//PID calculations
	float currentVelocity;
	int prevPosition;
	int error;
	int prevError;
	int derivative;

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
	float outVal;
} vel_PID;

/**
 * Initializes a velocity PID controller
 *
 * @param pid The PID controller
 * @param kP Proportional gain
 * @param kD Derivative gain
 * @param ticksPerRev Sensor ticks per one revolution
 */
void vel_PID_InitController(vel_PID *pid, const float kP, const float kD, const float ticksPerRev);

/**
 * Sets new filter constants
 *
 * @param pid The PID controller
 * @param alpha DEMA alpha gain
 * @param beta DEMA beta gain
 */
inline void vel_PID_SetFilterConstants(vel_PID *pid, const float alpha, const float beta);

/**
 * Sets the controller's target velocity
 *
 * @param pid The PID controller
 * @param targetVelocity New target velocity
 */
inline void vel_PID_SetTargetVelocity(vel_PID *pid, const int targetVelocity);

/**
 * Gets the current error
 *
 * @param pid The PID controller
 */
inline int vel_PID_GetError(vel_PID *pid);

/**
 * Gets the current (filtered) velocity
 *
 * @param pid The PID controller
 */
inline float vel_PID_GetVelocity(vel_PID *pid);

/**
 * Gets the current target velocity
 *
 * @param pid The PID controller
 */
inline float vel_PID_GetTargetVelocity(vel_PID *pid);

/**
 * Gets the current output
 *
 * @param pid The PID controller
 */
inline int vel_PID_GetOutput(vel_PID *pid);

/**
 * Steps the controller's velocity calculation without stepping math
 *
 * @param pid The PID controller
 * @param sens New sensor reading
 */
int vel_PID_StepVelocity(vel_PID *pid, const float sens);

/**
 * Steps the controller's calculations
 *
 * @param pid The PID controller
 * @param sens New sensor reading
 */
int vel_PID_StepController(vel_PID *pid, const float sens);

#endif
