#ifndef VEST_OBJ_H
#define VEST_OBJ_H

#include "main.h"

#define VEST_MESSAGE_LENGTH 128

static char vest_message[VEST_MESSAGE_LENGTH];

static int vest_new_message_received_flag;

static uint32_t r_pick_counter;

static char aux_message[64];

#endif // VEST_OBJ_H
