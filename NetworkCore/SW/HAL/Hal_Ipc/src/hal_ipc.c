/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

 #include <zephyr/ipc/ipc_service.h>

 #include "hal_ipc.h"
 #include "hal_ipc_cfg.h"
 
 /***********************************************************************************************************
  ************************************************* Macros **************************************************
  ***********************************************************************************************************/
 
 /***********************************************************************************************************
  *********************************************** Data types ************************************************
  ***********************************************************************************************************/
 
 /***********************************************************************************************************
  **************************************** Local function prototypes ****************************************
  ***********************************************************************************************************/
 
 static void Hal_Ipc_Bound(void *priv);
 static void Hal_Ipc_Received(const void *data, size_t len, void *priv);
 static void Hal_Ipc_Error(const char *message, void *priv);
 
 /***********************************************************************************************************
  ******************************************** Exported objects *********************************************
  ***********************************************************************************************************/
 
 /***********************************************************************************************************
  ********************************************* Local objects ***********************************************
  ***********************************************************************************************************/
 
  /* IPC endpoint configuration structure */
  static struct ipc_ept_cfg Hal_Ipc_Config = {
     .name = "ep0",
     .cb = {
         .bound = Hal_Ipc_Bound,
         .received = Hal_Ipc_Received,
         .error = Hal_Ipc_Error,
     },
 };
 
 const struct device *Hal_Ipc_Instance = DEVICE_DT_GET(DT_NODELABEL(ipc0));
 static struct ipc_ept Hal_Ipc_Endpoint;
 
 /* Buffer for storing received IPC data */
 static uint8_t Hal_Ipc_ReceivedDataBuf[HAL_IPC_RECEIVED_DATA_BUF_SIZE];
 
 /***********************************************************************************************************
  ******************************************* Exported functions ********************************************
  ***********************************************************************************************************/
 
  /**
  * @brief Initialize the IPC module by opening an IPC instance and registering an endpoint.
  * @param None
  * @return SYSTEM_OK if initialization is successful, otherwise SYSTEM_NOK.
  */
 System_Ret_t Hal_Ipc_Init(void)
 {
     System_Ret_t ret = SYSTEM_NOK;
 
     if(0 == ipc_service_open_instance(Hal_Ipc_Instance))
     {
         if(0 == ipc_service_register_endpoint(Hal_Ipc_Instance, &Hal_Ipc_Endpoint, &Hal_Ipc_Config))
         {
             ret = SYSTEM_OK;
         }
     }
 
     return ret;
 }
 
 /**
  * @brief Send data over the IPC channel.
  *
  * @param len Length of the data to send.
  * @param data Pointer to the data buffer.
  * @return SYSTEM_OK if the data is sent successfully, otherwise SYSTEM_NOK.
  */
 System_Ret_t Hal_Ipc_Send(uint8_t len, uint8_t* data)
 {
     System_Ret_t ret = SYSTEM_NOK;
 
     if(0 > ipc_service_send(&Hal_Ipc_Endpoint, data, len))
     {
         ret = SYSTEM_OK;
     }
 
     return ret;
 }
 
 /***********************************************************************************************************
  ******************************************** Local functions **********************************************
  ***********************************************************************************************************/
 
  /**
  * @brief Callback executed when the IPC endpoint is bound.
  *
  * @param priv Private data pointer (unused).
  */
 static void Hal_Ipc_Bound(void *priv)
 {
     /* Do nothing */
 }
 
 /**
  * @brief Callback executed when data is received over IPC.
  *
  * @param data Pointer to received data.
  * @param len Length of the received data.
  * @param priv Private data pointer (unused).
  */
 static void Hal_Ipc_Received(const void *data, size_t len, void *priv)
 {
     memcpy(Hal_Ipc_ReceivedDataBuf, (uint8_t*)data, len);
     Hal_Ipc_ReceivedCb(len, Hal_Ipc_ReceivedDataBuf);
 }
 
 /**
  * @brief Callback executed when an error occurs in IPC communication.
  *
  * @param message Error message.
  * @param priv Private data pointer (unused).
  */
 static void Hal_Ipc_Error(const char *message, void *priv)
 {
     /* Do nothing */
 }
 