#ifndef USART2_BUFFER_OBJ_INTERFACE_H
#define USART2_BUFFER_OBJ_INTERFACE_H

void usart2_buffer_obj_write(char data);
char usart2_buffer_obj_read();
char usart2_buffer_obj_read_last();
void usart2_buffer_obj_write_reset();
void usart2_buffer_obj_read_reset();

void usart2_object_init();

void usart2_new_byte_flag_set();
void usart2_new_byte_flag_reset();
int usart2_new_byte_flag_get();

void usart2_buffer_action();

void usart2_buffer_get_message(char *message_buffer);

#endif // USART2_BUFFER_OBJ_INTERFACE_H
