
#include "ctrs.h"

#define THREAD_COUNT 2
#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY RT_THREAD_PRIORITY_MAX / 3
#define THREAD_TICK 10

static void receiver_handler(void* parameter)
{
	char *data = NULL;
	rt_uint16_t length = 0;
	
	while (1) {
        // 接收输入消息
		topic_receive(topic_test, &data, &length);

		if(length && data)
		{
			rt_kprintf("Received input data: %s\n", data);
			
			// 释放输入消息的数据指针
	        rt_free(data);

			data = NULL;
			length = 0;
		}
		rt_thread_mdelay(1);
    }
}

int receiver_init(void)
{
	// 订阅主题
	topic_subscribe(topic_test);

    // 创建接收消息处理线程
    rt_thread_t thread = rt_thread_create("receiver", receiver_handler, NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TICK);
    rt_thread_startup(thread);

	return RT_EOK;
}
INIT_APP_EXPORT(receiver_init);


