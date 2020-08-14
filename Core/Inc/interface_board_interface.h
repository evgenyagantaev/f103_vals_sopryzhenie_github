#ifndef INTERFACE_BOARD_INTERFACE_H
#define INTERFACE_BOARD_INTERFACE_H

void interface_board_object_init();

void interface_board_new_message_received_flag_set();
void interface_board_new_message_received_flag_reset();
int interface_board_new_message_received_flag_get();

void interface_board_action();
void impact_action();

#endif  // INTERFACE_BOARD_INTERFACE_H
