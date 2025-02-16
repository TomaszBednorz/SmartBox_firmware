#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "hal_ble.h"
#include "hal_ipc.h"

#define MY_STACK_SIZE 512
#define MY_PRIORITY 7

void test_thread(void);

int main(void)
{
	Hal_Ipc_Init();
	Hal_Ble_Init();

	Hal_Ble_StartAdvertising();

	return 0;
}

// Generis msg : ID LEN DATA
// LEDS message: ID LEN R  G  B (16-bit little endian)
// LEDS message: 00 06  x  y  z

void test_thread(void)
{
	uint16_t cnt = 0;

	while(1)
	{	
		cnt++;
		printk("%d\n", cnt);
		k_sleep(K_MSEC(1000));

	}
}

K_THREAD_DEFINE(thread1, MY_STACK_SIZE, test_thread, NULL, NULL, NULL, MY_PRIORITY, 0, 0);

