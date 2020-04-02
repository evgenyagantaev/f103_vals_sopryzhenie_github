#include "vest_obj.h"
#include "vest_interface.h"
#include "usart2_buffer_interface.h"

void vest_new_message_received_flag_set()
{
	vest_new_message_received_flag = 1;
}


void vest_new_message_received_flag_reset()
{
	vest_new_message_received_flag = 0;
}
int vest_new_message_received_flag_get()
{
	return vest_new_message_received_flag;
}

void vest_object_init()
{
	vest_new_message_received_flag_reset();
}


void vest_action()
{
	if(vest_new_message_received_flag_get())
	{
		vest_new_message_received_flag_reset();

		usart2_buffer_get_message(vest_message);

		int i = 0;
		i++;
	}
}

