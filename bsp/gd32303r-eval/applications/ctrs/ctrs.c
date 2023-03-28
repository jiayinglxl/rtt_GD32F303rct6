
#include "ctrs.h"

subscriber_t *sub_head = RT_NULL;

rt_err_t topic_receive(topic_t topic, char **buffer, rt_uint16_t *length)
{
	char topic_name[16];
	message_t msg = {0};
	
	rt_snprintf(topic_name, sizeof(topic_name), "topic_%d", topic);
	
	rt_mq_t queue = (rt_mq_t)rt_object_find(topic_name, RT_Object_Class_MessageQueue);

	if(queue == RT_NULL)
	{
		rt_kprintf("No %s subscribe\r\n", topic_name);
		return RT_ERROR;
	}
	
    rt_err_t ret = rt_mq_recv(queue, &msg, sizeof(message_t), RT_WAITING_FOREVER);

	if(ret != RT_EOK)
	{
		rt_kprintf("mq receive error\r\n");
		return RT_ERROR;
	}
	
	if(topic == msg.topic)
	{
		*buffer = msg.data;
		*length = msg.lenght;
	}

	return RT_EOK;
}

/* 取消订阅 */
rt_int8_t topic_unsubscribe(topic_t topic)
{
	subscriber_t *sub = sub_head;
	subscriber_t *sub_pre = sub_head;
	
	rt_thread_t thread_id = rt_thread_self();
	
	while(sub)
	{
		if((thread_id == sub->thread_id) && (sub->topic == topic))
		{
			if(sub == sub_head)
			{
				sub_head = sub->next;
			}
			else
			{
				sub_pre->next = sub->next;
			}

			rt_kprintf("Unsubscribe topic %d\r\n",sub->topic);
			
			rt_mutex_delete(sub->mutex);
			rt_mq_delete(sub->mq);
			rt_free(sub);
			
			break;
		}
		sub_pre = sub;
		sub = sub->next;
	}

	return RT_EOK;
}

/* 订阅主题 */
rt_int8_t topic_subscribe(topic_t topic)
{
	// 线程重复订阅主题
	subscriber_t *tmp = sub_head;
	rt_thread_t thread_id = rt_thread_self();

	while(tmp)
	{
		if((thread_id == tmp->thread_id) && (tmp->topic == topic))
		{
			rt_kprintf("repeat subscribe topic %d\r\n",topic);
			return RT_EOK;
		}
		tmp = tmp->next;
	}
	
	subscriber_t *sub = (subscriber_t *)rt_malloc(sizeof(subscriber_t));

	char topic_name[16];
    rt_snprintf(topic_name, sizeof(topic_name), "topic_%d", topic);
	
	sub->topic = topic;
	sub->mutex = rt_mutex_create(topic_name, RT_IPC_FLAG_FIFO);
	sub->mq = rt_mq_create(topic_name, sizeof(message_t), 16, RT_IPC_FLAG_FIFO);
	sub->thread_id = thread_id;
	sub->next = RT_NULL;

	sub->next = sub_head;
	sub_head = sub;

	return RT_EOK;
}


/* 发布主题 */
rt_int8_t topic_publish(topic_t topic, void *buffer, rt_uint16_t length)
{
	subscriber_t *sub = sub_head;
	message_t msg;

	while(sub)
	{
//		rt_kprintf("publish sub_head point addr %x\n", sub);
		if(sub->topic == topic)
		{
			rt_mutex_take(sub->mutex, RT_WAITING_FOREVER);
			
			msg.lenght = length;
			msg.topic = topic;
			msg.data = (char *)rt_malloc(length);			// 防止数据丢失
			rt_memcpy(msg.data, (char *)buffer, length);
			
			rt_mq_send(sub->mq, &msg, sizeof(msg));

			rt_mutex_release(sub->mutex);
		}
		
		sub = sub->next;
	}

	return RT_EOK;
}

