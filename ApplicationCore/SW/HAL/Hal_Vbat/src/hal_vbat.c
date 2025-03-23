/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

#include "hal_vbat.h"

/***********************************************************************************************************
 ************************************************* Macros **************************************************
 ***********************************************************************************************************/

#define HAL_VBAT_VBAT_MEAS_PIN_ENABLE      (1)
#define HAL_VBAT_VBAT_MEAS_PIN_DISABLE     (0)

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

static const struct gpio_dt_spec Hal_Vbat_PinVbatMeasEn = GPIO_DT_SPEC_GET(DT_NODELABEL(vbatt), power_gpios);
static const struct adc_dt_spec Hal_Vbat_AdcChannel = ADC_DT_SPEC_GET(DT_NODELABEL(vbatt));

static int16_t Hal_Vbat_Buf;
static struct adc_sequence Hal_Vbat_Sequence = {
    .buffer = &Hal_Vbat_Buf,
    .buffer_size = sizeof(Hal_Vbat_Buf),
};

/***********************************************************************************************************
 ******************************************* Exported functions ********************************************
 ***********************************************************************************************************/

/**
 * @brief Initialize the VBAT hardware abstraction layer.
 * 
 * @return System_Ret_t SYSTEM_OK if initialization is successful, otherwise SYSTEM_NOK.
 */
System_Ret_t Hal_Vbat_Init(void)
{
    System_Ret_t ret = SYSTEM_OK;

    /* Initialize VBAT enable pin */
    if(true != device_is_ready(Hal_Vbat_PinVbatMeasEn.port))
    {
        ret = SYSTEM_NOK;
    }

    if(0 > gpio_pin_configure_dt(&Hal_Vbat_PinVbatMeasEn, GPIO_OUTPUT_LOW))
    {
        ret = SYSTEM_NOK;
    }

    /* Initialize VBAT ADC channel */
    if (!adc_is_ready_dt(&Hal_Vbat_AdcChannel))
    {
        ret = SYSTEM_NOK;
    }

    if (adc_channel_setup_dt(&Hal_Vbat_AdcChannel) < 0) 
    {
        ret = SYSTEM_NOK;
    }

    if (adc_sequence_init_dt(&Hal_Vbat_AdcChannel, &Hal_Vbat_Sequence) < 0) 
    {
        ret = SYSTEM_NOK;
    }

    return ret;
}

/**
 * @brief Trigger a VBAT measurement.
 * 
 * @param buf Pointer to store the measurement result.
 * @return System_Ret_t SYSTEM_OK if measurement is successful, otherwise SYSTEM_NOK.
 */
System_Ret_t Hal_Vbat_TrigMeasurement(uint16_t *buf)
{
    System_Ret_t ret = SYSTEM_NOK;

    gpio_pin_set_dt(&Hal_Vbat_PinVbatMeasEn, HAL_VBAT_VBAT_MEAS_PIN_ENABLE);   

    if (0 == adc_read(Hal_Vbat_AdcChannel.dev, &Hal_Vbat_Sequence)) 
    {
        *buf = (uint16_t)Hal_Vbat_Buf;
        ret = SYSTEM_OK;
    }

    gpio_pin_set_dt(&Hal_Vbat_PinVbatMeasEn, HAL_VBAT_VBAT_MEAS_PIN_DISABLE);

    return ret;
}

/***********************************************************************************************************
 ******************************************** Local functions **********************************************
 ***********************************************************************************************************/
