#pragma once
#include "platform/system.h"

typedef struct {
    float phy_walk_drag;
    uint64_t demo_npc_move_speed;
    uint64_t demo_npc_steer_speed;
    float item_pick_radius;
    float item_merger_radius;
    float item_attract_radius;
    float item_attract_force;
    float item_container_reach_radius;
    float item_drop_pickup_time;
    float item_drop_merger_time;
    float plr_move_speed;
    float plr_move_speed_mult;
    float vehicle_force;
    float vehicle_accel;
    float vehicle_decel;
    float vehicle_steer;
    float vehicle_steer_compensation;
    float vehicle_steer_revert;
    float vehicle_power;
    float vehicle_brake_force;
    float veh_enter_radius;
    float blueprint_build_time;

	// survival rules
} game_rulesdef;

extern game_rulesdef game_rules;

void rules_setup();
