#include "usart1_buffer_obj.h"
#include "usart1_buffer_interface.h"
#include "interface_board_interface.h"

void usart1_buffer_obj_write_reset()
{
	usart1_buffer_write_index = 0;
}

void usart1_buffer_obj_read_reset()
{
	usart1_buffer_read_index = 0;
}

void usart1_buffer_obj_write(char data)
{

	if(usart1_buffer_write_index < USART1_BUFFER_LENGTH)
	{
		usart1_buffer[usart1_buffer_write_index] = data;
		usart1_buffer_write_index++;
	}
}

char usart1_buffer_obj_read()
{
	char return_data = usart1_buffer[usart1_buffer_read_index];
	usart1_buffer_read_index++;

	return return_data;
}

char usart1_buffer_obj_read_last()
{
	char return_data = usart1_buffer[(usart1_buffer_write_index - 1)];

	return return_data;
}

void usart1_new_byte_flag_set()
{
	usart1_new_byte_flag = 1;
}
void usart1_new_byte_flag_reset()
{
	usart1_new_byte_flag = 0;
}
int usart1_new_byte_flag_get()
{
	return usart1_new_byte_flag;
}

void usart1_object_init()
{
	usart1_new_byte_flag_reset();
	usart1_buffer_obj_write_reset();
	usart1_buffer_obj_read_reset();
}


void usart1_buffer_action()
{
	if(usart1_new_byte_flag_get())
	{
		usart1_new_byte_flag_reset();

		if(usart1_buffer_obj_read_last() == '\n')
		{
			interface_board_new_message_received_flag_set();
		}
	}
}

void usart1_buffer_get_message(char *message_buffer)
{
	if(usart1_buffer_write_index < USART1_BUFFER_LENGTH)
	{
		usart1_buffer[usart1_buffer_write_index] = 0;

		int i;

		for(i=0; i<=usart1_buffer_write_index; i++)
			message_buffer[i] = usart1_buffer[i];

		usart1_buffer_obj_write_reset();
	}
}

