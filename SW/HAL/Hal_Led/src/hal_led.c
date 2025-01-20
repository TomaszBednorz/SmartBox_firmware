/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

#include <zephyr/drivers/pwm.h>

#include "hal_led.h"
#include "hal_led_cfg.h"

/***********************************************************************************************************
 ************************************************* Macros **************************************************
 ***********************************************************************************************************/

/**
 * @brief Set the period and pulse width in nanoseconds.
 *
 * @param color RGB led color, see @HAL_LEC_COLOR.
 * @param period Period (in nanoseconds).
 * @param pulse Pulse width (in nanoseconds).
 * @return A value from pwm_set_dt().
 */
#define Hal_Led_SetPeriodAndPulse(color, period, pulse)  \
    pwm_set_dt(&Hal_Led_PwmLed[color], period, pulse)

/**
 * @brief Set the pulse width in nanoseconds.
 *
 * @param color RGB led color, @HAL_LEC_COLOR.
 * @param pulse Pulse width (in nanoseconds).
 * @return A value from pwm_set_pulse_dt().
 */
#define Hal_Led_SetPulse(color, pulse)  \
    pwm_set_pulse_dt(&Hal_Led_PwmLed[color], pulse)

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

static const struct pwm_dt_spec Hal_Led_PwmLed[HAL_LED_MAX] = 
{
    PWM_DT_SPEC_GET(DT_NODELABEL(red_led_pwm)),
    PWM_DT_SPEC_GET(DT_NODELABEL(green_led_pwm)),
    PWM_DT_SPEC_GET(DT_NODELABEL(blue_led_pwm))
};

/***********************************************************************************************************
 ******************************************* Exported functions ********************************************
 ***********************************************************************************************************/

/**
 * @brief HAL LED module initialization function. LED hardware is initialized,
 *        the default period is configured, and the initial LED duty cycle is set to 0.
 *
 * @param None
 * @return SYSTEM_OK if initialization is successful, otherwise SYSTEM_NOK.
 */
System_Ret_t Hal_Led_Init(void)
{
    System_Ret_t ret = SYSTEM_OK;

    for(uint8_t i = 0; i < (uint8_t)HAL_LED_MAX; i++)
    {
        if(true != pwm_is_ready_dt(&Hal_Led_PwmLed[i]))
        {
            ret = SYSTEM_NOK;
        }

        if(true != pwm_is_ready_dt(&Hal_Led_PwmLed[i]))
        {
            ret = SYSTEM_NOK;
        }

        if(0 != Hal_Led_SetPeriodAndPulse((Hal_Led_Color_t)i, HAL_LED_CFG_DEFAULT_PERIOD, 0U))
        {
            ret = SYSTEM_NOK;
        }
    }

    return ret;
}

/**
 * @brief Set the LED duty cycle for the specified LED (RGB color).
 *
 * @param color RGB color to configure, see @HAL_LEC_COLOR.
 * @param duty_cycle Duty cycle in the range 0 - 10000 (0 - 100%).
 * @return SYSTEM_OK if the duty cycle has been set successfully, otherwise SYSTEM_NOK.
 */
System_Ret_t Hal_Led_SetDutyCycle(const Hal_Led_Color_t color, const uint16_t duty_cycle)
{
    System_Ret_t ret = SYSTEM_NOK;
    uint32_t pulse;
    
    if(duty_cycle <= HAL_LED_DUTY_CYCLE_MAX)
    {
        pulse = HAL_LED_CFG_DEFAULT_PERIOD / HAL_LED_DUTY_CYCLE_MAX;
        pulse *= duty_cycle;

        if(0 == Hal_Led_SetPulse(color, pulse))
        {
            ret = SYSTEM_OK;
        }
    }

    return ret;
}

/***********************************************************************************************************
 ******************************************** Local functions **********************************************
 ***********************************************************************************************************/
