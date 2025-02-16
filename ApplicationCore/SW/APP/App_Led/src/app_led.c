/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

#include "app_led.h"
#include "app_led_cfg.h"
#include "hal_led.h"

/***********************************************************************************************************
 ************************************************* Macros **************************************************
 ***********************************************************************************************************/

/* Mask for LED event data ready */
#define APP_LED_EVENT_DATA_RDY_MASK     (0x100)

/***********************************************************************************************************
 *********************************************** Data types ************************************************
 ***********************************************************************************************************/

/*
 * The type contains the LEDs brightness as a PWM duty cycle in the range from 0 (0%) to 10000 (100%).
 */
typedef struct
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
}App_Led_Brightness_t;

/***********************************************************************************************************
 **************************************** Local function prototypes ****************************************
 ***********************************************************************************************************/

static void App_Led_ThreadProcessData(void *unused1, void *unused2, void *unused3);

/***********************************************************************************************************
 ******************************************** Exported objects *********************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ********************************************* Local objects ***********************************************
 ***********************************************************************************************************/

static App_Led_Brightness_t App_Led_BrightnessActual;
static App_Led_Brightness_t App_Led_BrightnessNew;

static struct k_event App_Led_EventDataRdy;
static struct k_mutex App_Led_MutexDataProtect;

/* Thread definition for LED data processing */
K_THREAD_DEFINE(App_Led_Thread, APP_LED_THREAD_STACKSIZE, App_Led_ThreadProcessData, NULL, NULL, NULL, APP_LED_THREAD_PRIORITY, 0U, 0U);

/***********************************************************************************************************
 ******************************************* Exported functions ********************************************
 ***********************************************************************************************************/

/**
 * @brief Initialize the LED application module.
 *        This function initializes the event for data availability indication
 *        and a matex to protect LED data.
 * @param None
 * @return None
 */
System_Ret_t App_Led_Init(void)
{
    System_Ret_t ret = SYSTEM_OK;

    k_event_init(&App_Led_EventDataRdy);   

    if(0 != k_mutex_init(&App_Led_MutexDataProtect))
    {
        ret = SYSTEM_NOK;
    }

    return ret;
}

/**
 * @brief Set the RGB color for the LEDs.
 * 
 * @param red Brightness of the red LED from 0 (0%) to 10000 (100%).
 * @param green Brightness of the green LED from 0 (0%) to 10000 (100%).
 * @param blue Brightness of the blue LED from 0 (0%) to 10000 (100%).
 * @return None
 */
System_Ret_t App_Led_SetLedRgbColor(uint16_t red, uint16_t green, uint16_t blue)
{
    System_Ret_t ret = SYSTEM_NOK;
    
    /* Protect data from concurrent access, see: @APP_LED_MUTEX_NOTE */
    if(0 == k_mutex_lock(&App_Led_MutexDataProtect, K_MSEC(20)))
    {
        App_Led_BrightnessNew.red = red;
        App_Led_BrightnessNew.green = green;
        App_Led_BrightnessNew.blue = blue;

        if(0 == k_mutex_unlock(&App_Led_MutexDataProtect))
        {
            k_event_post(&App_Led_EventDataRdy, APP_LED_EVENT_DATA_RDY_MASK);
            ret = SYSTEM_OK;
        }
    }

    return ret;
}

/***********************************************************************************************************
 ******************************************** Local functions **********************************************
 ***********************************************************************************************************/

/**
 * @brief Thread function to process and apply LED brightness updates.
 *        This function waits for an event indicating new LED data is ready, then updates the brightness 
 *        values for the red, green, and blue LEDs as necessary.
 * 
 * @param unused1 Unused parameter.
 * @param unused2 Unused parameter.
 * @param unused3 Unused parameter.
 * @return None
 */
static void App_Led_ThreadProcessData(void *unused1, void *unused2, void *unused3)
{
    uint32_t events;

    while(1)
    {
        events = k_event_wait(&App_Led_EventDataRdy, APP_LED_EVENT_DATA_RDY_MASK, true, K_FOREVER);
        
        if (APP_LED_EVENT_DATA_RDY_MASK == events) 
        {
            /* 
            * Protect data from concurrent access.
            * Concurrent access to members of the App_Led_BrightnessActual structure is forbidden.
            * The mutex protects against situations where the duty cycle is updated, and preemption
            * occurs right after it, before the value in App_Led_BrightnessActual is updated.
            * ref: @APP_LED_MUTEX_NOTE
            */
            if(0 == k_mutex_lock(&App_Led_MutexDataProtect, K_FOREVER))
            {
                /* Update the red LED */
                if(App_Led_BrightnessNew.red != App_Led_BrightnessActual.red)
                {
                    if(SYSTEM_OK == Hal_Led_SetDutyCycle(HAL_LED_RED, App_Led_BrightnessNew.red))
                    {
                        App_Led_BrightnessActual.red = App_Led_BrightnessNew.red;
                    }
                }
                
                /* Update the green LED */
                if(App_Led_BrightnessNew.green != App_Led_BrightnessActual.green)
                {
                    if(SYSTEM_OK == Hal_Led_SetDutyCycle(HAL_LED_GREEN, App_Led_BrightnessNew.green))
                    {
                        App_Led_BrightnessActual.green = App_Led_BrightnessNew.green;
                    }
                }

                /* Update the blue LED */
                if(App_Led_BrightnessNew.blue != App_Led_BrightnessActual.blue)
                {
                    if(SYSTEM_OK == Hal_Led_SetDutyCycle(HAL_LED_BLUE, App_Led_BrightnessNew.blue))
                    {
                        App_Led_BrightnessActual.blue = App_Led_BrightnessNew.blue;
                    }
                }

                if(0 != k_mutex_unlock(&App_Led_MutexDataProtect))
                {
                    /* This situation shall never happen. */
                    SYSTEM_ERR("APP LED: Mutex can't be unlocked");
                }
            }
        }
    }
}
