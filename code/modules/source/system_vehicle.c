#include "debug_draw.h"

#define VEH_ENTER_RADIUS 45.0f

void LeaveVehicle(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    IsInVehicle *vehp = ecs_column(it, IsInVehicle, 2);
    Velocity *v = ecs_column(it, Velocity, 3);
    
    for (int i = 0; i < it->count; i++) {
        if (!in[i].use) continue;
        
        Vehicle *veh = 0;
        if ((veh = ecs_get_mut_if(it->world, vehp->veh, Vehicle))) {
            for (int k = 0; k < 4; k++) {
                if (veh->seats[k] == it->entities[i]) {
                    veh->seats[k] = 0;
                    break;
                }
            }
            
            in[i].use = false;
            ecs_remove(it->world, it->entities[i], IsInVehicle);
            
            // NOTE(zaklaus): push passenger out
            {
                float px = zpl_sin(veh->heading)*400.0f;
                float py = zpl_cos(veh->heading)*400.0f;
                v->x += px;
                v->y += py;
            }
        } else {
            ZPL_PANIC("unreachable code");
        }
    }
}

void EnterVehicle(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Position *p = ecs_column(it, Position, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (!in[i].use) continue;
        
        size_t ents_count;
        int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);
        
        for (size_t j = 0; j < ents_count; j++) {
            Vehicle *veh = 0;
            if ((veh = ecs_get_mut_if(it->world, ents[j], Vehicle))) {
                Position const* p2 = ecs_get(it->world, ents[j], Position);
                
                float dx = p2->x - p[i].x;
                float dy = p2->y - p[i].y;
                float range = zpl_sqrt(dx*dx + dy*dy);
                if (range <= VEH_ENTER_RADIUS) {
                    for (int k = 0; k < 4; k++) {
                        if (veh->seats[k] != 0) continue;
                        
                        // NOTE(zaklaus): We can enter the vehicle, yay!
                        veh->seats[k] = it->entities[i];
                        ecs_set(it->world, it->entities[i], IsInVehicle, {
                                    .veh = ents[j]
                                });
                        p[i] = *p2;
                        in[i].use = false;
                        break;
                    }
                }
            }
        }
    }
}

#define VEHICLE_FORCE 34.8f
#define VEHICLE_ACCEL 0.02f
#define VEHICLE_DECEL 0.28f
#define VEHICLE_STEER 0.09f
#define VEHICLE_BRAKE_FORCE 0.04f

void VehicleHandling(ecs_iter_t *it) {
    Vehicle *veh = ecs_column(it, Vehicle, 1);
    Position *p = ecs_column(it, Position, 2);
    Velocity *v = ecs_column(it, Velocity, 3);
    
    for (int i = 0; i < it->count; i++) {
        Vehicle *car = &veh[i];
        
        // NOTE(zaklaus): Apply friction
        car->steer *= 0.97f;
        
        for (int j = 0; j < 4; j++) {
            // NOTE(zaklaus): Perform seat cleanup
            if (!world_entity_valid(veh[i].seats[j])) {
                veh[i].seats[j] = 0;
                continue;
            }
            
            ecs_entity_t pe = veh[i].seats[j];
            
            // NOTE(zaklaus): Handle driver input
            if (j == 0) {
                Input const* in = ecs_get(it->world, pe, Input);
                
                car->force += zpl_lerp(0.0f, in->y * VEHICLE_FORCE, VEHICLE_ACCEL);
                if (in->sprint) {
                    car->force = zpl_lerp(car->force, 0.0f, VEHICLE_BRAKE_FORCE);
                    
                    if (zpl_abs(car->force) < 5.5f) 
                        car->force = 0.0f;
                }
                car->steer += in->x * VEHICLE_STEER;
                car->steer = zpl_clamp(car->steer, -40.0f, 40.0f);
            }
        }
        
        car->force = zpl_clamp(car->force, car->reverse_speed, car->speed);
        
        // NOTE(zaklaus): Vehicle physics
        float fr_x = p[i].x + (car->wheel_base/2.0f) * zpl_cos(car->heading);
        float fr_y = p[i].y + (car->wheel_base/2.0f) * zpl_sin(car->heading);
        
        float bk_x = p[i].x - (car->wheel_base/2.0f) * zpl_cos(car->heading);
        float bk_y = p[i].y - (car->wheel_base/2.0f) * zpl_sin(car->heading);
        
        world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y);
        float drag = zpl_clamp(blocks_get_drag(lookup.block_id), 0.0f, 1.0f);
        
        bk_x += car->force * drag * zpl_cos(car->heading);
        bk_y += car->force * drag * zpl_sin(car->heading);
        fr_x += car->force * drag * zpl_cos(car->heading + zpl_to_radians(car->steer));
        fr_y += car->force * drag * zpl_sin(car->heading + zpl_to_radians(car->steer));
        
        v[i].x += (fr_x + bk_x) / 2.0f - p[i].x;
        v[i].y += (fr_y + bk_y) / 2.0f - p[i].y;
        car->heading = zpl_arctan2(fr_y - bk_y, fr_x - bk_x);
        
        for (int j = 0; j < 4; j++) {
            if (!world_entity_valid(veh[i].seats[j])) continue;
            ecs_entity_t pe = veh[i].seats[j];
            
            // NOTE(zaklaus): Update passenger position
            {
                Position *p2 = ecs_get_mut(it->world, pe, Position, NULL);
                Velocity *v2 = ecs_get_mut(it->world, pe, Velocity, NULL);
                *p2 = p[i];
                *v2 = v[i];
            }
        }
        
        {
            debug_v2 b2 = {p[i].x + zpl_cos(car->heading)*(car->wheel_base), p[i].y + zpl_sin(car->heading)*(car->wheel_base)};
            debug_push_line((debug_v2){p[i].x, p[i].y}, b2, 0x0000FFFF);
            
            {
                float dx = zpl_cos(car->heading);
                float dy = zpl_sin(car->heading);
                debug_push_circle((debug_v2){p[i].x+dx*car->force, p[i].y+dy*car->force}, 5.0f, 0x00FF00FF);
            }
        }
    }
}

void ClearVehicle(ecs_iter_t *it) {
    Vehicle *veh = ecs_column(it, Vehicle, 1);
    
    for (int i = 0; i < it->count; i++) {
        for (int k = 0; k < 4; k++) {
            if (world_entity_valid(veh[i].seats[k])) {
                ecs_remove(it->world, veh[i].seats[k], IsInVehicle);
            }
        }
    }
}
