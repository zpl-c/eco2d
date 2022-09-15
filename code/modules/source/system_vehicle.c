#include "debug_draw.h"
#include "entity.h"

#define VEH_ENTER_RADIUS 45.0f

void LeaveVehicle(ecs_iter_t *it) {
    Input *in = ecs_field(it, Input, 1);
    IsInVehicle *vehp = ecs_field(it, IsInVehicle, 2);
    Velocity *v = ecs_field(it, Velocity, 3);

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
                float px = zpl_cos(veh->heading)*400.0f;
                float py = zpl_sin(veh->heading)*400.0f;
                v->x += py;
                v->y -= px;
            }
        } else {
            ZPL_PANIC("unreachable code");
        }
    }
}

void EnterVehicle(ecs_iter_t *it) {
    Input *in = ecs_field(it, Input, 1);
    Position *p = ecs_field(it, Position, 2);

    for (int i = 0; i < it->count; i++) {
        if (!in[i].use) continue;

        size_t ents_count;
        int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);
        bool has_entered_veh = false;

        for (size_t j = 0; j < ents_count; j++) {
            Vehicle *veh = 0;

            if (has_entered_veh) break;

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
                        has_entered_veh = true;
                        break;
                    }
                }
            }
        }
    }
}

#define VEHICLE_FORCE 240.8f
#define VEHICLE_ACCEL 0.032f
#define VEHICLE_DECEL 0.28f
#define VEHICLE_STEER 35.89f
#define VEHICLE_STEER_COMPENSATION 4.0f
#define VEHICLE_STEER_REVERT 6.0941816f
#define VEHICLE_POWER 97.89f
#define VEHICLE_BRAKE_FORCE 0.84f

void VehicleHandling(ecs_iter_t *it) {
    Vehicle *veh = ecs_field(it, Vehicle, 1);
    Position *p = ecs_field(it, Position, 2);
    Velocity *v = ecs_field(it, Velocity, 3);

    for (int i = 0; i < it->count; i++) {
        Vehicle *car = &veh[i];

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

                car->force += zpl_lerp(0.0f, in->y * VEHICLE_FORCE, (zpl_sign(in->y) == zpl_sign(car->force) ? 1.0f : 3.0f) * VEHICLE_ACCEL*safe_dt(it));
                if (in->sprint) {
                    car->force = zpl_lerp(car->force, 0.0f, VEHICLE_BRAKE_FORCE*safe_dt(it));

                    if (zpl_abs(car->force) < 5.5f)
                        car->force = 0.0f;
                }
                float steer_mod = (1 - zpl_abs(car->force) / car->speed) + VEHICLE_STEER_COMPENSATION * safe_dt(it);
                car->steer = zpl_lerp(car->steer, 0.0f, safe_dt(it)*VEHICLE_STEER_REVERT);
                car->steer += (in->x * VEHICLE_STEER * steer_mod)*safe_dt(it);
                car->steer = zpl_clamp(car->steer, -60.0f, 60.0f);
            }
        }

        car->force = zpl_clamp(car->force, car->reverse_speed, car->speed);

        // NOTE(zaklaus): Vehicle physics
        float fr_x = p[i].x + (car->wheel_base/2.0f) * zpl_cos(car->heading);
        float fr_y = p[i].y + (car->wheel_base/2.0f) * zpl_sin(car->heading);

        float bk_x = p[i].x - (car->wheel_base/2.0f) * zpl_cos(car->heading);
        float bk_y = p[i].y - (car->wheel_base/2.0f) * zpl_sin(car->heading);

        world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y);
        float drag = zpl_clamp(blocks_get_drag(lookup.bid), 0.0f, 1.0f);

        bk_x += car->force * drag * zpl_cos(car->heading) * safe_dt(it)*VEHICLE_POWER;
        bk_y += car->force * drag * zpl_sin(car->heading) * safe_dt(it)*VEHICLE_POWER;
        fr_x += car->force * drag * zpl_cos(car->heading + zpl_to_radians(car->steer)) * safe_dt(it)*VEHICLE_POWER;
        fr_y += car->force * drag * zpl_sin(car->heading + zpl_to_radians(car->steer)) * safe_dt(it)*VEHICLE_POWER;

        v[i].x += ((fr_x + bk_x) / 2.0f - p[i].x);
        v[i].y += ((fr_y + bk_y) / 2.0f - p[i].y);
        car->heading = zpl_arctan2(fr_y - bk_y, fr_x - bk_x);

        float check_x = p[i].x+PHY_LOOKAHEAD(v[i].x);
        float check_y = p[i].y+PHY_LOOKAHEAD(v[i].y);
        world_block_lookup lookahead = world_block_from_realpos(check_x, check_y);
        uint32_t flags = blocks_get_flags(lookahead.bid);
        if (flags & BLOCK_FLAG_COLLISION) {
            if (flags & BLOCK_FLAG_DESTROY_ON_COLLISION) {
                world_chunk_destroy_block(check_x, check_y, true);
                car->force *= 0.8f;
            } else {
                car->force = 0.0f;
            }
        }

        for (int j = 0; j < 4; j++) {
            if (!world_entity_valid(veh[i].seats[j])) continue;
            ecs_entity_t pe = veh[i].seats[j];

            // NOTE(zaklaus): Update passenger position
            {
                Velocity *v2 = ecs_get_mut(it->world, pe, Velocity);
                entity_set_position(pe, p[i].x, p[i].y);
                *v2 = v[i];
            }
        }

        if (zpl_abs(car->force) > ENTITY_ACTION_VELOCITY_THRESHOLD) {
            entity_wake(it->entities[i]);
        }

        {
            debug_v2 b2 = {p[i].x + zpl_cos(car->heading)*(car->wheel_base), p[i].y + zpl_sin(car->heading)*(car->wheel_base)};
            debug_push_line((debug_v2){p[i].x, p[i].y}, b2, 0x0000FFFF);

            // NOTE(zaklaus): force
            {
                float dx = zpl_cos(car->heading);
                float dy = zpl_sin(car->heading);
                debug_push_circle((debug_v2){p[i].x+dx*car->force, p[i].y+dy*car->force}, 5.0f, 0x00FF00FF);
            }

            // NOTE(zaklaus): steer
            {
                float dx = zpl_sin(car->heading);
                float dy = -zpl_cos(car->heading);
                float steer_mult = -80.0f;
                debug_push_circle((debug_v2){p[i].x+dx*car->steer*steer_mult, p[i].y+dy*car->steer*steer_mult}, 5.0f, 0x00FFAAFF);
            }
        }
    }
}

void ClearVehicle(ecs_iter_t *it) {
    Vehicle *veh = ecs_field(it, Vehicle, 1);

    for (int i = 0; i < it->count; i++) {
        for (int k = 0; k < 4; k++) {
            if (world_entity_valid(veh[i].seats[k])) {
                ecs_remove(it->world, veh[i].seats[k], IsInVehicle);
            }
        }
    }
}
