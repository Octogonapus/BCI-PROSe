#include "filter.h"

/**
 * Initializes an exponential moving average filter
 *
 * @param filter The EMA filter
 */
void filter_Init_EMA(EMAFilter *filter)
{
	filter->output = 0.0;
	filter->output_old = 0.0;
}

/**
 * Filters an input
 *
 * @param filter The EMA filter
 * @param readIn Input to filter
 * @param alpha EMA alpha gain
 */
float filter_EMA(EMAFilter *filter, const float readIn, const float alpha)
{
	filter->output = alpha * readIn + (1.0 - alpha) * filter->output_old;
	filter->output_old = filter->output;
	return filter->output;
}

/**
 * Initializes a double exponential moving average filter
 *
 * @param filter The DEMA filter
 */
void filter_Init_DEMA(DEMAFilter *filter)
{
	filter->outputS = 0.0;
	filter->outputB = 0.0;
	filter->outputS_old = 0.0;
	filter->outputB_old = 0.0;
}

/**
 * Filters an input
 *
 * @param filter The DEMA filter
 * @param readIn Input to filter
 * @param alpha DEMA alpha gain
 * @param beta DEMA beta gain
 */
float filter_DEMA(DEMAFilter *filter, const float readIn, const float alpha, const float beta)
{
	filter->outputS = (alpha * readIn) + ((1.0 - alpha) * (filter->outputS_old + filter->outputB_old));
	filter->outputB = (beta * (filter->outputS - filter->outputS_old)) + ((1.0 - beta) * filter->outputB_old);
	filter->outputS_old = filter->outputS;
	filter->outputB_old = filter->outputB;

	return filter->outputS + filter->outputB;
}

/**
 * Five-unit average filter
 *
 * @param filter The FUA filter
 */
void filter_Init_FUA(FUAFilter *filter)
{
	filter->index = 0;

	for (int i = 0; i < 5; i++)
	{
		filter->components[i] = 0;
	}
}

/**
 * Filters an input
 *
 * @param filter The FUA filter
 * @param componentIn Input to filter
 */
float filter_FUA(FUAFilter *filter, const float componentIn)
{
	filter->components[filter->index] = componentIn;
	filter->index = filter->index + 1 > 4 ? 0 : filter->index + 1;

	float avg = 0.0;

	for (int i = 0; i < 5; i++)
	{
		avg += filter->components[i];
	}

	return avg / 5.0;
}

/**
 * Ten-unit average filter
 *
 * @param filter The TUA filter
 */
void filter_Init_TUA(TUAFilter *filter)
{
	filter->index = 0;

	for (int i = 0; i < 10; i++)
	{
		filter->components[i] = 0;
	}
}

/**
 * Filters an input
 *
 * @param filter The TUA filter
 * @param componentIn Input to filter
 */
float filter_TUA(TUAFilter *filter, const float componentIn)
{
	filter->components[filter->index] = componentIn;
	filter->index = filter->index + 1 > 9 ? 0 : filter->index + 1;

	float avg = 0.0;

	for (int i = 0; i < 10; i++)
	{
		avg += filter->components[i];
	}

	return avg / 10.0;
}
