
#include <rtthread.h>
#include <string.h>

#define THREAD_COUNT 2
#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY RT_THREAD_PRIORITY_MAX / 3
#define THREAD_TICK 10

typedef enum
{
	topic_test = 0,
}topic_t;

// 订阅者结构
typedef struct sub
{
	topic_t topic;
	rt_mq_t mq;
	
	struct sub *next;
}sub_t;

// 发布者结构
typedef struct pub
{
	topic_t topic;
	rt_int8_t *data;
	rt_uint8_t lenght;
}pub_t;

sub_t *sub_head = NULL;

/* 订阅主题 */
rt_int8_t topic_subscribe(sub_t *sub)
{
	if(sub == NULL)
	{
		return RT_ERROR;
	}
	
	sub->next = sub_head;
	sub_head = sub;
}


/* 发布主题 */
rt_int8_t topic_publish(pub_t *pub)
{
	sub_t *sub = sub_head;
	pub_t pub_tmp;
	
	while(sub)
	{
		if(sub->topic == pub->topic)
		{
			pub_tmp.lenght = pub->lenght;
			pub_tmp.topic = pub->topic;
			pub_tmp.data = (char *)rt_malloc(pub->lenght);
			rt_memcpy(pub_tmp.data, pub->data, pub->lenght);
			rt_mq_send(sub->mq, &pub_tmp, sizeof(pub_t));
		}
		
		sub = sub->next;
	}
}

static void queue_topic_handler(void* parameter)
{
	while (1) {
        // 接收输入消息
        pub_t pub;
		rt_mq_t input_queue = (rt_mq_t)rt_object_find("queue_topic", RT_Object_Class_MessageQueue);
        rt_mq_recv(input_queue, &pub, sizeof(pub_t), RT_WAITING_FOREVER);

		rt_kprintf("Received input data: %s\n", pub.data);

		// 释放输入消息的数据指针
        rt_free(pub.data);
    }
}

void queue_topic_init(void) {

	sub_t *sub = (sub_t *)rt_malloc(sizeof(sub_t));
	rt_mq_t queue = rt_mq_create("queue_topic", sizeof(pub_t), 16, RT_IPC_FLAG_FIFO);
	
	sub->topic = topic_test;
	sub->mq = queue;
	sub->next = NULL;

    // 订阅主题
    topic_subscribe(sub);

    // 创建输入消息处理线程
    rt_thread_t thread = rt_thread_create("queue_topic_handler", queue_topic_handler, NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TICK);
    rt_thread_startup(thread);
}


// 应用程序入口
int queue_topic_test(int argc, char **argv) {
	// 初始化主题队列
	queue_topic_init();
	
	// 发送输入数据
	pub_t pub;
	pub.topic = topic_test;
	pub.data = "Input data 1";
	pub.lenght = strlen("Input data 1") + 1;
		
	topic_publish(&pub);
	
	// 等待输入消息处理完成
	rt_thread_mdelay(100);
	
	return 0;
}
MSH_CMD_EXPORT(queue_topic_test, test topic);


