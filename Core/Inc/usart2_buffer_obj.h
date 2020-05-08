#ifndef USART2_BUFFER_OBJ_H
#define USART2_BUFFER_OBJ_H

#include "main.h"

#define USART2_BUFFER_LENGTH 128

static char usart2_buffer[USART2_BUFFER_LENGTH];
static int usart2_buffer_write_index;
static int usart2_buffer_read_index;
static int usart2_buffer_message_length;

static int usart2_new_byte_flag;

#endif // USART2_BUFFER_OBJ_H
