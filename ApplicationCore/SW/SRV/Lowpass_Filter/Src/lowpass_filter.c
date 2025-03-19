/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

#include "lowpass_filter.h"

/***********************************************************************************************************
 ************************************************* Defines *************************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 *********************************************** Data types ************************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 **************************************** Local function prototypes ****************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ******************************************** Exported objects *********************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ********************************************* Local objects ***********************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ******************************************* Exported functions ********************************************
 ***********************************************************************************************************/

/**
 * @brief Initialize a first-order lowpass filter.
 * 
 * @param filter Pointer to the filter structure to initialize.
 * @param alpha Smoothing factor for the filter.
 */
void Lowpass_Filter_1stOrderInit(LowpassFilter_1stStage_t *filter, float alpha)
{
    filter->previous_output = 0.0f;
    filter->alpha = alpha;
}

/**
 * @brief Update the first-order lowpass filter with a new input value.
 * 
 * @param filter Pointer to the filter structure.
 * @param input New input value to filter.
 * @return float Filtered output value.
 */
float Lowpass_Filter_1stOrderUpdate(LowpassFilter_1stStage_t *filter, float input)
{
    float output = filter->alpha * input + (1.0f - filter->alpha) * filter->previous_output;
    filter->previous_output = output;
    return output;
}

/**
 * @brief Initialize a third-order lowpass filter.
 * 
 * @param filter Pointer to the filter structure to initialize.
 * @param alpha Smoothing factor for the filter stages.
 */
void Lowpass_Filter_3rdOrderInit(LowpassFilter_3rdStage_t *filter, float alpha)
{
    Lowpass_Filter_1stOrderInit(&filter->stage1, alpha);
    Lowpass_Filter_1stOrderInit(&filter->stage2, alpha);
    Lowpass_Filter_1stOrderInit(&filter->stage3, alpha);
}

/**
 * @brief Update the third-order lowpass filter with a new input value.
 * 
 * @param filter Pointer to the filter structure.
 * @param input New input value to filter.
 * @return float Filtered output value.
 */
float Lowpass_Filter_3rdOrderUpdate(LowpassFilter_3rdStage_t *filter, float input)
{
    float output = Lowpass_Filter_1stOrderUpdate(&filter->stage1, input);
    output = Lowpass_Filter_1stOrderUpdate(&filter->stage2, output);
    output = Lowpass_Filter_1stOrderUpdate(&filter->stage3, output);
    return output;
}

/***********************************************************************************************************
 ******************************************** Local functions **********************************************
 ***********************************************************************************************************/

