#ifndef FILTER_H_
#define FILTER_H_

//Exponential moving average filter
typedef struct EMAFilter_t
{
    float output;
    float output_old;
} EMAFilter;

//Double exponential moving average filter
typedef struct DEMAFilter_t
{
    float outputS;
    float outputB;
    float outputS_old;
    float outputB_old;
} DEMAFilter;

//Five-unit average filter
typedef struct FUAFilter_t
{
    float components[5];
    int index;
} FUAFilter;

//Ten-unit average filter
typedef struct TUAFilter_t
{
    float components[10];
    int index;
} TUAFilter;

/**
 * Initializes an exponential moving average filter
 *
 * @param filter The EMA filter
 */
void filter_Init_EMA(EMAFilter *filter);

/**
 * Filters an input
 *
 * @param filter The EMA filter
 * @param readIn Input to filter
 * @param alpha EMA alpha gain
 */
float filter_EMA(EMAFilter *filter, const float readIn, const float alpha);

/**
 * Initializes a double exponential moving average filter
 *
 * @param filter The DEMA filter
 */
void filter_Init_DEMA(DEMAFilter *filter);

/**
 * Filters an input
 *
 * @param filter The DEMA filter
 * @param readIn Input to filter
 * @param alpha DEMA alpha gain
 * @param beta DEMA beta gain
 */
float filter_DEMA(DEMAFilter *filter, const float readIn, const float alpha, const float beta);

/**
 * Five-unit average filter
 *
 * @param filter The FUA filter
 */
void filter_Init_FUA(FUAFilter *filter);

/**
 * Filters an input
 *
 * @param filter The FUA filter
 * @param componentIn Input to filter
 */
float filter_FUA(FUAFilter *filter, const float componentIn);

/**
 * Ten-unit average filter
 *
 * @param filter The TUA filter
 */
void filter_Init_TUA(TUAFilter *filter);

/**
 * Filters an input
 *
 * @param filter The TUA filter
 * @param componentIn Input to filter
 */
float filter_TUA(TUAFilter *filter, const float componentIn);

#endif
