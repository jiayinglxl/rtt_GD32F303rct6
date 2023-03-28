
#ifndef __CTRS_H__
#define __CTRS_H__

#include <rtthread.h>
#include <string.h>

typedef enum
{
	topic_test = 0,
	topic_app,
}topic_t;

// 订阅者结构
typedef struct subscriber
{
	topic_t topic;				// 主题
	rt_mq_t mq;
	rt_mutex_t mutex;
	rt_thread_t thread_id;
	
	struct subscriber *next;
}subscriber_t;

// 消息结构
typedef struct message
{
	topic_t topic;
	char *data;
	rt_uint16_t lenght;
}message_t;

#endif //__CTRS_H__