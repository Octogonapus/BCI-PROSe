#ifndef MATH_H_
#define MATH_H_

#define CUBED_127 16129
#define ROOT_2    1.414
#define EPSILON   1000
#define sign(value) ( (value) >= 0 ? 1 : (-1) )
#define cube(value) ( (value) * (value) * (value) )
#define inchesToTicks(inches, diam) ( ((inches) / (PI * (diam))) * 360 )
#define ticksToInches(ticks, diam) ( ((diam) * PI) * ((ticks) / 360) )

/**
 * Calculate timestep
 *
 * @param dt dt variable to save to
 * @param prevTime Previous time variable to pull from and then save to
 */
inline unsigned long util_CalculateTimestep(unsigned int *dt, unsigned int *prevTime)
{
	*dt = (millis() - *prevTime) / 1000.0;
	*prevTime = millis();

	return *dt;
}

#endif
