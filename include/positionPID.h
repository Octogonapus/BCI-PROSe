#ifndef POSITIONPID_H_
#define POSITIONPID_H_

//PID Controller representation
typedef struct pos_PID_t
{
	//PID constants
	float kP;
	float kI;
	float kD;
	float kBias;

	//PID calculations
	int error;
	int prevError;
	int integral;
	int derivative;

	//PID limits
	int errorThreshold;
	int integralLimit;

	//Timestep
	unsigned int dt;
	unsigned int prevTime;

	//Input
	int targetPos;

	//Output
	int outVal;
} pos_PID;

/**
 * Initializes a position PID controller
 *
 * @param pid The PID controller to initialize
 * @param kP Proportional gain
 * @param kI Integral gain
 * @param kD Derivative gain
 */
void pos_PID_InitController(pos_PID *pid, const float kP, const float kI, const float kD);

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
void pos_PID_InitController_Full(pos_PID *pid, const float kP, const float kI, const float kD, const float kBias, const int errorThreshold, const int integralLimit);

/**
 * Sets the target position
 *
 * @param pid The PID controller
 * @param targetPos Target position
 */
inline void pos_PID_SetTargetPosition(pos_PID *pid, const int targetPos);

/**
 * Gets the current error
 *
 * @param pid The
 */
inline int pos_PID_GetError(pos_PID *pid);

/**
 * Gets the current output
 *
 * @param pid The PID controller
 */
inline int pos_PID_GetOutput(pos_PID *pid);

/**
 * Steps the controller's calculations
 *
 * @param pid The pid controller
 * @param sens New sensor reading
 */
int pos_PID_StepController(pos_PID *pid, const float sens);

#endif
