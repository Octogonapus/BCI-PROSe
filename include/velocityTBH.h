#ifndef VELOCITYTBH_H_
#define VELOCITYTBH_H_

#include "filter.h"
#include "util.h"

//A velocity TBH controller
typedef struct vel_TBH_t
{
	//TBH constants
	float gain;

	//TBH calculations
	float currentVelocity;
	int currentPosition;
	int prevPosition;
	int error;
	int prevError;
	bool firstCross;
	int outValApprox;
	float outValAtZero;
	float outValChange;

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
} vel_TBH;

/**
 * Initializes a velocity TBH controller
 *
 * @param tbh The TBH controller
 * @param gain Controller gain
 * @param outValApprox Approximate output at zero error for a given target velocity
 * @param ticksPerRev Sensor ticks per one revolution
 */
void vel_TBH_InitController(vel_TBH *tbh, const float gain, const int outValApprox, const float ticksPerRev);

/**
 * Reinitializes a velocity TBH controller
 *
 * @param tbh The TBH controller to initialize
 */
void vel_TBH_ReInitController(vel_TBH *tbh);

/**
 * Sets new filter constants
 *
 * @param tbh The TBH controller
 * @param alpha DEMA filter constant alpha
 * @param beta DEMA filter constant beta
 */
inline void vel_TBH_SetFilterConstants(vel_TBH *tbh, const float alpha, const float beta);

/**
 * Sets the target velocity
 * This should (generally) be used when the target velocity has changed
 *
 * @param tbh The TBH controller
 * @param targetVelocity Target velocity
 * @param outValApprox Approximate output at zero error for this target velocity
 */
inline void vel_TBH_SetTargetVelocity(vel_TBH *tbh, const int targetVelocity, const int outValApprox);

/**
 * Gets the current error
 *
 * @param tbh The TBH controller
 */
inline int vel_TBH_GetError(vel_TBH *tbh);

/**
 * Gets the current (filtered) velocity
 *
 * @param tbh The TBH controller
 */
inline int vel_TBH_GetVelocity(vel_TBH *tbh);

/**
 * Gets the current target velocity
 *
 * @param tbh The TBH controller
 */
inline int vel_TBH_GetTargetVelocity(vel_TBH *tbh);

/**
 * Gets the current output
 *
 * @param tbh The TBH controller
 */
inline int vel_TBH_GetOutput(vel_TBH *tbh);

/**
 * Sets the open-loop approximation
 *
 * @param tbh The TBH controller
 * @param outValApprox New approximate output for current targetVelocity
 */
inline void vel_TBH_SetOpenLoopApprox(vel_TBH *tbh, const int outValApprox);

/**
 * Gets the current open-loop approximation
 *
 * @param tbh The TBH controller
 */
inline int vel_TBH_GetOpenLoopApprox(vel_TBH *tbh);

/**
 * Steps the controller's velocity calculation without stepping math
 *
 * @param tbh The TBH controller
 * @param sens New sensor reading
 */
int vel_TBH_StepVelocity(vel_TBH *tbh, const float sens);

/**
 * Steps the controller calculations
 *
 * @param tbh The TBH controller
 * @param sens New sensor reading
 */
int vel_TBH_StepController(vel_TBH *tbh, const float sens);

#endif
