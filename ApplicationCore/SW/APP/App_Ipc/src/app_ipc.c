/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

#include "app_ipc.h"
#include "app_ipc_cb.h"
#include "app_led.h"

/***********************************************************************************************************
 ************************************************* Macros **************************************************
 ***********************************************************************************************************/

/* Indexes of fields in IPC message */
#define APP_IPC_MESSAGE_ID_IDX       (0U)
#define APP_IPC_MESSAGE_LEN_IDX      (1U)
#define APP_IPC_MESSAGE_PAYLOAD_IDX  (2U)

/* Indexes of fields in IPC message for LEDs */
#define APP_IPC_MESSAGE_LEDS_RED_IDX     (0U)
#define APP_IPC_MESSAGE_LEDS_GREEN_IDX   (2U)
#define APP_IPC_MESSAGE_LEDS_BLUE_IDX    (4U)

/***********************************************************************************************************
 *********************************************** Data types ************************************************
 ***********************************************************************************************************/

/**
 * This structure contains information about each IPC message instance, including its ID, mask, payload size, 
 * read/write permissions, and the received/transmitted data (payload).
 */
typedef struct {
    const uint8_t id;
    const uint32_t mask;
    const uint8_t len;
    const bool read_perm;
    const bool write_perm;
    uint8_t payload[APP_IPC_MAX_DATA_BUF_SIZE];
}App_Ipc_MessageInstance_t;

/***********************************************************************************************************
 **************************************** Local function prototypes ****************************************
 ***********************************************************************************************************/

static void App_Ipc_ThreadProcessData(void *unused1, void *unused2, void *unused3);
static void App_Ipc_ProcessLeds(void);

/***********************************************************************************************************
 ******************************************** Exported objects *********************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ********************************************* Local objects ***********************************************
 ***********************************************************************************************************/

static App_Ipc_MessageInstance_t App_Ipc_Instance[APP_IPC_TYPE_MAX] = {
    #define APP_IPC_GENERATE_INSTANCE(name, id, len, r, w) {id, (1U << id), len, r, w, {0}},
    APP_IPC_MESSAGE_CFG_TABLE(APP_IPC_GENERATE_INSTANCE)
    #undef APP_IPC_GENERATE_INSTANCE
};

static struct k_event App_Ipc_EventDataRdy;

/* Thread definition for IPC data processing */
K_THREAD_DEFINE(App_Ipc_Thread, APP_IPC_THREAD_STACKSIZE, App_Ipc_ThreadProcessData, NULL, NULL, NULL, APP_IPC_THREAD_PRIORITY, 0U, 0U);

/***********************************************************************************************************
 ******************************************* Exported functions ********************************************
 ***********************************************************************************************************/

/**
 * @brief Initializes the IPC module.
 *        This function initializes the event for data availability indication.
 *
 * @param None
 * @return SYSTEM_OK if initialization is successful, SYSTEM_NOT_OK otherwise.
 */
System_Ret_t App_Ipc_Init(void)
{
    System_Ret_t ret = SYSTEM_OK;

    k_event_init(&App_Ipc_EventDataRdy);   

    return ret;
}

System_Ret_t App_Ipc_Read(App_Ipc_MessageType_t type, uint8_t* data)
{
    // TODO
    return SYSTEM_NOK;
}

System_Ret_t App_Ipc_Write(App_Ipc_MessageType_t type, uint8_t* data)
{
    // TODO
    return SYSTEM_NOK;
}

/**
 * @brief Callback function for handling received IPC data.
 *        This callback function is called when IPC data is received. It processes the incoming data,
 *        checks the message ID, and if the data length matches the expected length, it stores the payload 
 *        in the corresponding IPC instance.
 * 
 * @param len The length of the received data.
 * @param data Pointer to the received data buffer.
 */
void App_Ipc_ReceivedCb(uint8_t len, uint8_t* data)
{
    uint8_t id = data[APP_IPC_MESSAGE_ID_IDX];
    uint8_t payload_len = data[APP_IPC_MESSAGE_LEN_IDX];

    for(uint8_t i = 0; i < (uint8_t)APP_IPC_TYPE_MAX; i++)
    {
        if(id == App_Ipc_Instance[i].id)
        {
            if(payload_len == App_Ipc_Instance[i].len)
            {
                memcpy(App_Ipc_Instance[i].payload, &data[APP_IPC_MESSAGE_PAYLOAD_IDX], payload_len);
                k_event_post(&App_Ipc_EventDataRdy, App_Ipc_Instance[i].mask);
                break;
            }
        }
    }
}

/***********************************************************************************************************
 ******************************************** Local functions **********************************************
 ***********************************************************************************************************/

/**
 * @brief Processes IPC data in a dedicated thread.
 *        This function waits for events signaling that data has been received, then processes the data accordingly.
 *        It handles different types of IPC messages.
 * 
 * @param unused1 Unused parameter.
 * @param unused2 Unused parameter.
 * @param unused3 Unused parameter.
 * @return None
 */
static void App_Ipc_ThreadProcessData(void *unused1, void *unused2, void *unused3)
{
    uint32_t events = 0;

    while(1)
    {
        events = k_event_wait(&App_Ipc_EventDataRdy, 0xFFFFFFFF, true, K_FOREVER);
        
        if (App_Ipc_Instance[APP_IPC_TYPE_LEDS].mask & events) 
        {
            App_Ipc_ProcessLeds();
        }
    }
}

/**
 * @brief Processes LED color configuration.
 *        This function reads the RGB color values from the payload and applies them to the LEDs.
 *
 * @param None
 * @return None
 */
static void App_Ipc_ProcessLeds(void)
{
    uint16_t red = *(uint16_t*)&App_Ipc_Instance[APP_IPC_TYPE_LEDS].payload[APP_IPC_MESSAGE_LEDS_RED_IDX];
    uint16_t green = *(uint16_t*)&App_Ipc_Instance[APP_IPC_TYPE_LEDS].payload[APP_IPC_MESSAGE_LEDS_GREEN_IDX];
    uint16_t blue = *(uint16_t*)&App_Ipc_Instance[APP_IPC_TYPE_LEDS].payload[APP_IPC_MESSAGE_LEDS_BLUE_IDX];

    if(SYSTEM_OK != App_Led_SetLedRgbColor(red, green, blue))
    {
        SYSTEM_ERR("APP IPC: Unsuccessful LEDs color configuration");
    }
}
