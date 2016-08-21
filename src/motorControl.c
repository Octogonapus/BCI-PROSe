#include "API.h"
#include "motorControl.h"
#include "timer.h"

//Array for motors
static driveMotor driveMotors[MOTOR_NUM];

/*
 * Sets the speed of the motor at index `index` to power `power`
 */
void setMotorSpeed(const unsigned char index, const int power)
{
	timer t;
	timer_GetDTFromStart(&t);
	driveMotors[index].reqSpeed = power;
}

/*
 * Sets the speed of the motor at index `index` to power `power`, and bypasses slewing
 * This is the controller-friendly way to bypass slew rates
 */
void setMotorSpeed_Bypass(const unsigned char index, const int power)
{
	driveMotors[index].reqSpeed = power;
	driveMotors[index].artSpeed = power;
}

/*
 * Sets the slew rate of the motor at index `index` to slew rate `rate`
 */
void setMotorSlew(const unsigned char index, const int rate)
{
	driveMotors[index].slew = rate;
}

/*
 * Gets the motor at index `index`
 */
driveMotor* getMotor(const unsigned char index)
{
	return &(driveMotors[index]);
}

/*
 * Gets the requested speed of the motor at index `index`
 */
int getMotorSpeed(const unsigned char index)
{
	return driveMotors[index].reqSpeed;
}

/*
 * Sets the motor at index `index` to active
 */
void setMotorActive(const unsigned char index)
{
	driveMotors[index].active = true;
}

/*
 * Sets the motor at index `index` to inactive
 */
void setMotorInactive(const unsigned char index)
{
	driveMotors[index].active = false;
}

/*
 * Sets the raw speed of the motor at index `index` to power `power`
 * Warning: The slew rate controller may try to fight this function
 * If you want to bypass slewing, use function `setMotorSpeed_Bypass` instead (or use this
 * function in combination with the function `setMotorInactive`)
 */
inline void setMotorSpeedRaw(const unsigned char index, const int power)
{
	motorSet(index, power);
}

/*
 * Gets the raw speed of the motor at index `index`
 */
inline int getMotorSpeedRaw(const unsigned char index)
{
	return motorGet(index);
}

/**
 * Adds and initializes a motor (by name) to the motor array
 *
 * @param index Port number of the motor
 * @param slewRate Slew rate of the motor
 */
driveMotor* addMotor(const unsigned char index, const float slewRate)
{
	/*
	 * Grab a spot in the motor array which corresponds with which port the physical motor is
	 * plugged into
	 */
	driveMotor *m = &(driveMotors[index]);

	m->reqSpeed = 0;
	m->artSpeed = 0;
	m->slew = slewRate;
	m->active = true;

	return m;
}

/**
 * Updates the power of each motor to best match the requested power
 */
static void motorSlewRateTask()
{
	//Index of current motor
	unsigned char motorIndex;

	//Requested speed of current motor
	int motorTmpReq;

	//Artifical speed of current motor
	float motorTmpArtSpd;

	//Current motor
	driveMotor *currentMotor;

	//Batch motor power update
	for (motorIndex = 0; motorIndex < MOTOR_NUM; motorIndex++)
	{
		/*
		 * Assign the driveMotor at motorIndex to currentMotor and check the active flag:
		 * if the motor is not active, do not let the controller update the motor's power
		 * by skipping that motor's update cycle
		 */
		if (!(currentMotor = &(driveMotors[motorIndex]))->active)
		{
			continue;
		}

		//Keep internal memory access to a minimum
		motorTmpArtSpd = currentMotor->artSpeed;
		motorTmpReq = currentMotor->reqSpeed;

		//If the motor value needs to change
		if (motorTmpArtSpd != motorTmpReq)
		{
			//Increase motor value
			if (motorTmpReq > motorTmpArtSpd)
			{
				motorTmpArtSpd += currentMotor->slew;

				//Limit speed
				motorTmpArtSpd = motorTmpArtSpd > motorTmpReq ? motorTmpReq : motorTmpArtSpd;
			}
			//Decrease motor value
			else if (motorTmpReq < motorTmpArtSpd)
			{
				motorTmpArtSpd -= currentMotor->slew;

				//Limit speed
				motorTmpArtSpd = motorTmpArtSpd < motorTmpReq ? motorTmpReq : motorTmpArtSpd;
			}

			//Bound speed
			motorTmpArtSpd = motorTmpArtSpd > MOTOR_MAX_VALUE ? MOTOR_MAX_VALUE : motorTmpArtSpd;
			motorTmpArtSpd = motorTmpArtSpd < MOTOR_MIN_VALUE ? MOTOR_MIN_VALUE : motorTmpArtSpd;

			//Send updated speed to motor
			motorSet(motorIndex, (int)motorTmpArtSpd);

			//Send updated speed back to current motor
			currentMotor->artSpeed = motorTmpArtSpd;
		}
	}
}

/*
 * Starts the motor slew rate task
 */
void startMotorSlewRateTask()
{
	taskRunLoop(motorSlewRateTask, 20);
}
