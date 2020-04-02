#ifndef VEST_INTERFACE_H
#define VEST_INTERFACE_H

void vest_object_init();

void vest_new_message_received_flag_set();
void vest_new_message_received_flag_reset();
int vest_new_message_received_flag_get();

void vest_action();

#endif  // VEST_INTERFACE_H
