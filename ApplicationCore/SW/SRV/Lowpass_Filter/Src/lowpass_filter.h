#ifndef _LOWPASS_FILTER_H_
#define _LOWPASS_FILTER_H_

/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ************************************************* Defines *************************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 *********************************************** Data types ************************************************
 ***********************************************************************************************************/

/**
 * @brief Structure for a single stage of a first-order lowpass filter.
 */
typedef struct {
    float previous_output; /**< Previous output value of the filter stage */
    float alpha;           /**< Smoothing factor for the filter stage */
} LowpassFilter_1stStage_t;

/**
 * @brief Structure for a third-order lowpass filter.
 */
typedef struct {
    LowpassFilter_1stStage_t stage1;
    LowpassFilter_1stStage_t stage2;
    LowpassFilter_1stStage_t stage3;
} LowpassFilter_3rdStage_t;

/***********************************************************************************************************
 ********************************************* Exported objects ********************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ************************************** Exported function prototypes ***************************************
 ***********************************************************************************************************/

void Lowpass_Filter_1stOrderInit(LowpassFilter_1stStage_t *filter, float alpha);
float Lowpass_Filter_1stOrderUpdate(LowpassFilter_1stStage_t *filter, float input);
void Lowpass_Filter_3rdOrderInit(LowpassFilter_3rdStage_t *filter, float alpha);
float Lowpass_Filter_3rdOrderUpdate(LowpassFilter_3rdStage_t *filter, float input);

#endif  /* _LOWPASS_FILTER_H_ */
