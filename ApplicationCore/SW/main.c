#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "hal_led.h"
#include "app_led.h"

#define MY_STACK_SIZE 512
#define MY_PRIORITY 7

void test_thread(void);

int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD_TARGET);

	Hal_Led_Init();

	App_Led_Init();

	for (;;) {
		k_sleep(K_MSEC(1000));
	}

	return 0;
}

void test_thread(void)
{
	uint16_t cnt = 0;

	while(1)
	{	
		cnt++;

		printk("%d\n", cnt);

		App_Led_SetLedRgbColor(cnt * 100, cnt * 100, cnt * 100);

		k_sleep(K_MSEC(100));
	}
}

K_THREAD_DEFINE(thread1, MY_STACK_SIZE, test_thread, NULL, NULL, NULL, MY_PRIORITY, 0, 0);
