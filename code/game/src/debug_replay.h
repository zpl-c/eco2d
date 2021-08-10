#pragma once
#include "system.h"
#include "packets/pkt_send_keystate.h"

void debug_replay_record_keystate(pkt_send_keystate state);
void debug_replay_update(void);