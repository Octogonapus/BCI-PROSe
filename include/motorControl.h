#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

#include <stdbool.h>

//Motor general
#define MOTOR_NUM               10     //Used to be kNumbOfTotalMotors, changed for now because
                                       //that could result in a bug where a motor is not accessible
#define MOTOR_MIN_VALUE         (-127) //Minimum power
#define MOTOR_MAX_VALUE         127    //Maximum power
#define MOTOR_DEFAULT_SLEW_RATE 10     //Feels like nearly no slewing to a driver
#define MOTOR_FAST_SLEW_RATE    256    //No slewing in output
#define MOTOR_TASK_DELAY        15     //Wait 15ms between batch motor power updates

//Motor representation
typedef struct driveMotor_t
{
	int reqSpeed;   //Input speed
	float artSpeed; //Artifical speed (the exact speed as governed by the slew rate)
	float slew;     //Slew rate
	bool active;    //Whether or not to update this motor
} driveMotor;

/*
 * Sets the speed of the motor at index `index` to power `power`
 */
inline void setMotorSpeed(const unsigned char index, const int power);
/*
 * Sets the speed of the motor at index `index` to power `power`, and bypasses slewing
 * This is the controller-friendly way to bypass slew rates
 */
inline void setMotorSpeed_Bypass(const unsigned char index, const int power);
/*
 * Sets the slew rate of the motor at index `index` to slew rate `rate`
 */
inline void setMotorSlew(const unsigned char index, const int rate);
/*
 * Gets the motor at index `index`
 */
inline driveMotor* getMotor(const unsigned char index);
/*
 * Gets the requested speed of the motor at index `index`
 */
inline int getMotorSpeed(const unsigned char index);
/*
 * Sets the motor at index `index` to active
 */
inline void setMotorActive(const unsigned char index);
/*
 * Sets the motor at index `index` to inactive
 */
inline void setMotorInactive(const unsigned char index);

/*
 * Sets the raw speed of the motor at index `index` to power `power`
 * Warning: The slew rate controller may try to fight this function
 * If you want to bypass slewing, use function `setMotorSpeed_Bypass` instead (or use this
 * function in combination with the function `setMotorInactive`)
 */
inline void setMotorSpeedRaw(const unsigned char index, const int power);
/*
 * Gets the raw speed of the motor at index `index`
 */
inline int getMotorSpeedRaw(const unsigned char index);

/**
 * Adds and initializes a motor (by name) to the motor array
 *
 * @param index Port number of the motor
 * @param slewRate Slew rate of the motor
 */
driveMotor* addMotor(const unsigned char index, const float slewRate);

/**
 * Updates the power of each motor to best match the requested power
 */
inline void startMotorSlewRateTask();

#endif
