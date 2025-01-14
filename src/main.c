#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define MY_STACK_SIZE 512
#define MY_PRIORITY 7

void test_thread(void);

int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD_TARGET);

	return 0;
}

void test_thread(void)
{
	uint16_t cnt = 0;

	while(1)
	{	
		cnt++;

		printk("Sleep for %d seconds ...\n", cnt);

		k_sleep(K_SECONDS(cnt));
	}
}

K_THREAD_DEFINE(thread1, MY_STACK_SIZE, test_thread, NULL, NULL, NULL, MY_PRIORITY, 0, 0);
