/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

#include "app_vbat.h"
#include "app_vbat_cfg.h"
#include "hal_vbat.h"
#include "app_ipc.h"
#include "lowpass_filter.h"

/***********************************************************************************************************
 ************************************************* Macros **************************************************
 ***********************************************************************************************************/

#define APP_VBAT_BATTERY_CURVE_LEN      (11U)
#define APP_VBAT_LAST_DECYL             (APP_VBAT_BATTERY_CURVE_LEN - 1U)
#define APP_VBAT_DECYL_FACTOR           (10.0f)

/**
 * @brief Converts a floating-point number to U8 format.
 */
#define App_Vbat_FloatToU8(num)        ((uint8_t)((num + 0.5f)))

/**
 * @brief Converts raw ADC value to millivolts.
 */
#define App_Vbat_RawToMiliVolts(raw) \
    (((float)(raw) / APP_VBAT_CFG_ADC_RESOLUTION) * APP_VBAT_CFG_ADC_MAX_VREF * APP_VBAT_CFG_SCALE_FACTOR)

/***********************************************************************************************************
 *********************************************** Data types ************************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 **************************************** Local function prototypes ****************************************
 ***********************************************************************************************************/

static void App_Vbat_ThreadProcessData(void *unused1, void *unused2, void *unused3);
static float App_Vbat_CalculateBatteryCondition(uint16_t raw_adc);
static float App_Vbat_EstimateBatteryPercent(float milivolts);

/***********************************************************************************************************
 ******************************************** Exported objects *********************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ********************************************* Local objects ***********************************************
 ***********************************************************************************************************/

static const float App_Vbat_BattCurve[APP_VBAT_BATTERY_CURVE_LEN] = APP_VBAT_CFG_BATTERY_CURVE;

static LowpassFilter_3rdStage_t App_Vbat_Measurement;

/* Thread definition for VBAT measurement & data processing */
K_THREAD_DEFINE(App_Vbat_Thread, APP_VBAT_THREAD_STACKSIZE, App_Vbat_ThreadProcessData, NULL, NULL, NULL, APP_VBAT_THREAD_PRIORITY, K_FP_REGS, 0U);

/***********************************************************************************************************
 ******************************************* Exported functions ********************************************
 ***********************************************************************************************************/

/**
 * @brief Initializes the VBAT application.
 */
void App_Vbat_Init(void)
{
    Lowpass_Filter_3rdOrderInit(&App_Vbat_Measurement, APP_VBAT_FILTER_ALFA);
}

/***********************************************************************************************************
 ******************************************** Local functions **********************************************
 ***********************************************************************************************************/

/**
 * @brief Thread function for processing VBAT data.
 */
static void App_Vbat_ThreadProcessData(void *unused1, void *unused2, void *unused3)
{
    uint16_t vbat_raw;
    uint8_t vbat_pr;
    
    while(1)
    {
        k_msleep(APP_VBAT_RHREAD_PERIOD);

        if(SYSTEM_OK == Hal_Vbat_TrigMeasurement(&vbat_raw))
        {
            vbat_pr = App_Vbat_CalculateBatteryCondition(vbat_raw);

            App_Ipc_Write(APP_IPC_TYPE_VBATT, &vbat_pr);
        }   
    }
}

/**
 * @brief Calculates the battery condition based on raw ADC value.
 * 
 * @param raw_adc Raw ADC value.
 * @return Battery condition as a percentage
 */
static float App_Vbat_CalculateBatteryCondition(uint16_t raw_adc)
{
    float vbat_raw, vbat_filtered, vbat_percent;
    
    vbat_raw = App_Vbat_RawToMiliVolts(raw_adc);

    vbat_filtered = Lowpass_Filter_3rdOrderUpdate(&App_Vbat_Measurement, vbat_raw);

    vbat_percent = App_Vbat_EstimateBatteryPercent(vbat_filtered);

    return App_Vbat_FloatToU8(vbat_percent);
}

/**
 * @brief Estimates the battery percentage based on millivolts.
 * 
 * @param milivolts Battery voltage in millivolts.
 * @return Battery percentage.
 */
static float App_Vbat_EstimateBatteryPercent(float milivolts)
{
    uint16_t decyl = 0U;
    float a, b, x, bat;

    if(milivolts > App_Vbat_BattCurve[APP_VBAT_LAST_DECYL])
    {
        /* Battery condition = 100% */
        bat = 100.0f;
    }
    else if(milivolts < App_Vbat_BattCurve[0U])
    {
        /* Battery condition = 0% */
        bat = 0.0f;
    }
    else
    {
        for(uint8_t i = 0; i < APP_VBAT_LAST_DECYL; i++)
        {
            if(milivolts < App_Vbat_BattCurve[i+1U])
            {
                decyl = i;
                break;
            }
        }

        a = App_Vbat_BattCurve[decyl];
        b = App_Vbat_BattCurve[decyl + 1U];

        x = (milivolts - a) / (b - a);

        bat = (decyl + x) * APP_VBAT_DECYL_FACTOR;
    }

    return bat;
}
