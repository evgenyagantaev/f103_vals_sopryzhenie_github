#ifndef INC_EXTERN_GLOBALS_H_
#define INC_EXTERN_GLOBALS_H_

#include "main.h"

extern char usart1_buffer[2][USART1_BUFFER_LENGTH];
extern int usart1_write_buffer;
extern int usart1_read_buffer;
extern int usart1_write_index;
extern int usart1_new_message_ready_flag;
extern int usart1_message_length;
extern int usart1_old_message_saved;
extern int usart1_message_lost;
extern int usart1_received_messages;
extern int usart1_processed_messages;

extern char usart2_buffer[2][USART2_BUFFER_LENGTH];
extern int usart2_write_buffer;
extern int usart2_read_buffer;
extern int usart2_write_index;
extern int usart2_new_message_ready_flag;
extern int usart2_message_length;
extern int usart2_old_message_saved;
extern int usart2_message_lost;
extern int usart2_received_messages;
extern int usart2_processed_messages;

extern char gps_coordinates[64];

#endif /* INC_EXTERN_GLOBALS_H_ */
