#ifndef UTIL_H_
#define UTIL_H_

#include "API.h"

//Enum for LED states
typedef enum
{
	LED_ON = 0,
	LED_OFF = 1
} LEDState;

//Quad encoder ticks per rev
#define UTIL_QUAD_TPR 360.0

//IME high torque ticks per rev
#define UTIL_IME_HT_TPR 627.2

//IME high speed ticks per rev
#define UTIL_IME_HS_TPR 392.0

//IME turbo gear ticks per rev
#define UTIL_IME_TG_TPR 261.333

//Wait for a value to become zero
#define waitForZero(val) while(val != 0) { delay(1); }

//Wait for any LCD button to be pressed
#define waitForLCDPress() while(lcdReadButtons(uart1) == 0) { delay(1); }

//Wait for all LCD buttons to be released
#define waitForLCDRelease() while(lcdReadButtons(uart1) != 0) { delay(1); }

#endif
