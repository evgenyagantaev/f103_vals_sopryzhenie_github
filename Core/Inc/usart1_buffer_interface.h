#ifndef USART1_BUFFER_OBJ_INTERFACE_H
#define USART1_BUFFER_OBJ_INTERFACE_H

void usart1_buffer_obj_write(char data);
char usart1_buffer_obj_read();
char usart1_buffer_obj_read_last();
void usart1_buffer_obj_write_reset();
void usart1_buffer_obj_read_reset();

void usart1_object_init();

void usart1_new_byte_flag_set();
void usart1_new_byte_flag_reset();
int usart1_new_byte_flag_get();

void usart1_buffer_action();

void usart1_buffer_get_message(char *message_buffer);

#endif
