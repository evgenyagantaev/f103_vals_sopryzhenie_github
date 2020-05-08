#ifndef USART1_BUFFER_OBJ_H
#define USART1_BUFFER_OBJ_H

#include "main.h"

//#define USART1_BUFFER_LENGTH 128

static char usart1_buffer[USART1_BUFFER_LENGTH];
static int usart1_buffer_write_index;
static int usart1_buffer_read_index;

static int usart1_new_byte_flag;

#endif // USART1_BUFFER_OBJ_H
