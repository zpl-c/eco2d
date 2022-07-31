#pragma once
#include "system.h"
#include "packet_utils.h"
#include "item_placement.h"

typedef struct {
    float x;
    float y;
    float mx;
    float my;
    uint8_t use;
    uint8_t sprint;
    uint8_t ctrl;
    uint8_t selected_item;
    uint8_t drop;
    uint8_t swap;
    uint8_t swap_from;
    uint8_t swap_to;
    
    // TODO(zaklaus): build mode
    uint8_t placement_num;
    item_placement placements[BUILD_MAX_PLACEMENTS];
} pkt_send_keystate;

typedef pkt_send_keystate game_keystate_data;

size_t pkt_send_keystate_send(uint16_t view_id,
                              game_keystate_data *data);
extern pkt_desc pkt_send_keystate_desc[];

PKT_HANDLER_PROC(pkt_send_keystate_handler);

