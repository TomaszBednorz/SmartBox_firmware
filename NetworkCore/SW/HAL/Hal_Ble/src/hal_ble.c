/***********************************************************************************************************
 ********************************************* Included files **********************************************
 ***********************************************************************************************************/

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>

#include "hal_ble.h"
#include "hal_ble_cfg.h"

/***********************************************************************************************************
 ************************************************* Macros **************************************************
 ***********************************************************************************************************/

#define HAL_BLE_DEVICE_NAME         CONFIG_BT_DEVICE_NAME
#define HAL_BLE_DEVICE_NAME_LEN     (sizeof(HAL_BLE_DEVICE_NAME) - 1)

/***********************************************************************************************************
 *********************************************** Data types ************************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 **************************************** Local function prototypes ****************************************
 ***********************************************************************************************************/

static void Hal_Ble_Connected(struct bt_conn *conn, uint8_t err);
static void Hal_Ble_Disconnected(struct bt_conn *conn, uint8_t reason);
static ssize_t Hal_Ble_WriteLeds(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, 
						 		 uint16_t len, uint16_t offset, uint8_t flags);

/***********************************************************************************************************
 ******************************************** Exported objects *********************************************
 ***********************************************************************************************************/

/***********************************************************************************************************
 ********************************************* Local objects ***********************************************
 ***********************************************************************************************************/

static struct bt_conn_cb Hal_Ble_ConnectionCallbacks = {
	.connected = Hal_Ble_Connected,
	.disconnected = Hal_Ble_Disconnected,
};

static const struct bt_le_adv_param *Hal_Ble_AdvertisingParameters = BT_LE_ADV_PARAM(
	(BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_IDENTITY),  /* Connectable advertising and use identity address */
	HAL_BLE_ADVERT_INTERVAL_MIN, HAL_BLE_ADVERT_INTERVAL_MAX,  /* Advertising interval */
	NULL);                                                     /* Set to NULL for undirected advertising */

static const struct bt_data Hal_Ble_AdvertisementPacket[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, HAL_BLE_DEVICE_NAME, HAL_BLE_DEVICE_NAME_LEN),
};

static const struct bt_data Hal_Ble_ScanResponsePacket[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, HAL_BLE_UUID_SERVICE_VAL),
};

/* 
 * BLE Service Declaration 
 */
BT_GATT_SERVICE_DEFINE(Hal_Ble_service,
BT_GATT_PRIMARY_SERVICE(HAL_BLE_UUID_SERVICE),
	BT_GATT_CHARACTERISTIC(HAL_BLE_UUID_LEDS_CHAR,
		BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, 
		NULL, Hal_Ble_WriteLeds, NULL),
);

/***********************************************************************************************************
 ******************************************* Exported functions ********************************************
 ***********************************************************************************************************/

System_Ret_t Hal_Ble_Init(void)
{
    System_Ret_t ret = SYSTEM_OK;

	if (0 != bt_enable(NULL)) 
    {
		ret = SYSTEM_NOK;
	}

	if (0 != bt_conn_cb_register(&Hal_Ble_ConnectionCallbacks)) 
    {
		ret = SYSTEM_NOK;
	}

    return ret;
}

System_Ret_t Hal_Ble_StartAdvertising(void)
{
    System_Ret_t ret = SYSTEM_OK;

	if (0 != bt_le_adv_start(Hal_Ble_AdvertisingParameters, 
                             Hal_Ble_AdvertisementPacket, ARRAY_SIZE(Hal_Ble_AdvertisementPacket), 
                             Hal_Ble_ScanResponsePacket, ARRAY_SIZE(Hal_Ble_ScanResponsePacket)))
    {
		ret = SYSTEM_NOK;
	}

    return ret;
}

System_Ret_t Hal_Ble_StopAdvertising(void)
{
    System_Ret_t ret = SYSTEM_OK;

	if (0 != bt_le_adv_stop())
    {
		ret = SYSTEM_NOK;
	}

    return ret;  
}

/***********************************************************************************************************
 ******************************************** Local functions **********************************************
 ***********************************************************************************************************/

static void Hal_Ble_Connected(struct bt_conn *conn, uint8_t err)
{
	Hal_Ble_ConnectedCb(conn, err);

	if (err) {
		SYSTEM_LOG("Connection failed, err 0x%02x \n", err);
		return;
	}
}

static void Hal_Ble_Disconnected(struct bt_conn *conn, uint8_t reason)
{
	Hal_Ble_DisconnectedCb(conn, reason);
}

static ssize_t Hal_Ble_WriteLeds(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, 
								 uint16_t len, uint16_t offset, uint8_t flags)
{
	ssize_t ret_val;

	if (len != 6U) {
		SYSTEM_ERR("Write led: Incorrect data length");
		ret_val = BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	else if (offset != 0) {
		SYSTEM_ERR("Write led: Incorrect data offset");
		ret_val = BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
	else
	{
		Hal_Ble_WriteLedsCb(buf, len);
		ret_val = len;
	}

	return ret_val;
}