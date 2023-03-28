
#include "ctrs.h"

#define THREAD_COUNT 2
#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY RT_THREAD_PRIORITY_MAX / 3
#define THREAD_TICK 10

static void transmiter_handler(void* parameter)
{
	char *data = NULL;
	rt_uint16_t length = 0;
	
	while (1) {
        // 接收输入消息
		topic_publish(topic_test, "Input data 1", (strlen("Input data 1")+1));
		rt_thread_mdelay(100);

		topic_publish(topic_test, "Input data 2", (strlen("Input data 2")+1));
		rt_thread_mdelay(100);
	
    }
}

int transmiter_init(void)
{
    // 创建接收消息处理线程
    rt_thread_t thread = rt_thread_create("transmiter", transmiter_handler, NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TICK);
    rt_thread_startup(thread);

	return RT_EOK;
}
INIT_APP_EXPORT(transmiter_init);


