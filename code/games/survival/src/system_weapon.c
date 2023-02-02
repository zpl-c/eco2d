ZPL_DIAGNOSTIC_PUSH_WARNLEVEL(0)
#include "tinyc2.h"
ZPL_DIAGNOSTIC_POP

#define WEAPON_KNIFE_SPAWN_DELAY 20
#define WEAPON_PROJECTILE_POS_OFFSET 200.0f
#define WEAPON_PROJECTILE_SPEED 500.0f
#define WEAPON_PROJECTILE_RANGE_LIFETIME 800.0f

//TODO(DavoSK): move to helpers, add srand
float get_rand_between(float min, float max) {
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

void WeaponKnifeMechanic(ecs_iter_t *it) {
	WeaponKnife *weapon = ecs_field(it, WeaponKnife, 1);
	const Position *pos = ecs_field(it, Position, 2);
    const Input *input = ecs_field(it, Input, 3);

	for (int i = 0; i < it->count; i++) {
		if (weapon[i].spawn_delay > 0) {
			TICK_VAR(weapon[i].spawn_delay);
            continue;
		}

        for(int j = 0; j < weapon[i].projectile_count; j++) {
            ecs_entity_t e = entity_spawn(EKIND_WEAPON);
            ecs_set(it->world, e, Sprite, { .spritesheet=0, .frame=2347 });
            ecs_set(it->world, e, TriggerOnly, { 0 });
            ecs_set(it->world, e, WeaponProjectile, { 
                .damage=weapon[i].damage,
                .origin_x=pos[i].x,
                .origin_y=pos[i].y
            });
            
            ecs_set(it->world, e, Rotation, { 
                .angle=zpl_to_degrees(zpl_arctan2(input[i].hx, input[i].hy)) 
            });

            ecs_set(it->world, e, Velocity, {
                .x=input[i].hx*WEAPON_PROJECTILE_SPEED,
                .y=input[i].hy*WEAPON_PROJECTILE_SPEED*-1
            });

            zpl_vec2 input_vec = {
                .x = input[i].hy,
                .y = input[i].hx
            };

            zpl_vec2 pos_offset;
            zpl_vec2_mul(&pos_offset, input_vec, get_rand_between(-WEAPON_PROJECTILE_POS_OFFSET, WEAPON_PROJECTILE_POS_OFFSET));     
            Position *dest = ecs_get_mut(world_ecs(), e, Position);
            dest->x = pos[i].x + pos_offset.x;
            dest->y = pos[i].y + pos_offset.y;       
        }

		weapon[i].spawn_delay = WEAPON_KNIFE_SPAWN_DELAY;
	}
}

void WeaponProjectileExpire(ecs_iter_t *it) {
    const WeaponProjectile *weapon = ecs_field(it, WeaponProjectile, 1);
	const Position *pos = ecs_field(it, Position, 2);

    for (int i = 0; i < it->count; i++) { 
        zpl_vec2 v_origin = { .x = weapon[i].origin_x, .y = weapon[i].origin_y };
        zpl_vec2 v_pos = { .x = pos[i].x, .y = pos[i].y };
        zpl_vec2 v_dist;
        zpl_vec2_sub(&v_dist, v_origin, v_pos);

        const float d = zpl_vec2_mag(v_dist);
        if(d > WEAPON_PROJECTILE_RANGE_LIFETIME) {
            entity_despawn(it->entities[i]);
        }
    }
}

zpl_vec2 rotate_point(float cx, float cy, float angle, zpl_vec2 p){
    return (zpl_vec2) {
        .x = zpl_cos(angle) * (p.x - cx) - zpl_sin(angle) * (p.y - cy) + cx,
        .y = zpl_sin(angle) * (p.x - cx) + zpl_cos(angle) * (p.y - cy) + cy
    };
}

void WeaponProjectileHit(ecs_iter_t *it) {
    const WeaponProjectile *weapon = ecs_field(it, WeaponProjectile, 1);
	const Position *pos = ecs_field(it, Position, 2);
    const Rotation *rot = ecs_field(it, Rotation, 3);

    for (int i = 0; i < it->count; i++) {
        ecs_iter_t it2 = ecs_query_iter(world_ecs(), ecs_mobpos_query);
        while (ecs_query_next(&it2)) {
            Mob *mob = ecs_field(&it2, Mob, 1);
            Position *mob_pos = ecs_field(&it2, Position, 2);
            
            for (int j = 0; j < it2.count; j++) {
                float p_x = pos[i].x;
                float p_y = pos[i].y;
                float p2_x = mob_pos[j].x /*+ v2[j].x*/;
                float p2_y = mob_pos[j].y /*+ v2[j].y*/;

                //NOTE(DavoSK): since our weapon has also rotation 
                //we need to rotate its bbox by rot
                float rot_radians = zpl_to_radians(rot[i].angle);
                zpl_vec2 bbox_min = rotate_point(p_x, p_y, rot_radians, (zpl_vec2){ 
                    .x = p_x - WORLD_BLOCK_SIZE / 2, 
                    .y = p_y - WORLD_BLOCK_SIZE / 4 
                });
                
                zpl_vec2 bbox_max = rotate_point(p_x, p_y, rot_radians, (zpl_vec2){ 
                    .x = p_x + WORLD_BLOCK_SIZE / 2, 
                    .y = p_y + WORLD_BLOCK_SIZE / 4 
                });

                c2AABB box_a = {
                    .min = { .x = bbox_min.x, .y = bbox_min.y },
                    .max = { .x = bbox_max.x, .y = bbox_max.y }
                };

                c2AABB box_b = {
                    .min = { p2_x - WORLD_BLOCK_SIZE / 2, p2_y - WORLD_BLOCK_SIZE / 4 },
                    .max = { p2_x + WORLD_BLOCK_SIZE / 2, p2_y + WORLD_BLOCK_SIZE / 4 },
                };

                float r1x = (box_a.max.x-box_a.min.x);
                float r1y = (box_a.max.y-box_a.min.y);
                float r1 = (r1x*r1x + r1y*r1y)*.5f;

                float r2x = (box_b.max.x-box_b.min.x);
                float r2y = (box_b.max.y-box_b.min.y);
                float r2 = (r2x*r2x + r2y*r2y)*.5f;

                {
                    float dx = (p2_x-p_x);
                    float dy = (p2_y-p_y);
                    float d = (dx*dx + dy*dy);

                    if (d > r1 && d > r2)
                        continue;
                }

                // c2Circle circle_a = { 
                //     .p = { p_x, p_y },
                //     .r = r1/2.f,
                // };

                // c2Circle circle_b = {
                //     .p = { p2_x, p2_y },
                //     .r = r2/2.f,
                // };


                // const void *shapes_a[] = { &circle_a, &box_a };
                // const void *shapes_b[] = { &circle_b, &box_b };
                
                c2Manifold m = { 0 };
                c2Collide(&box_a, 0, C2_TYPE_AABB, &box_b, 0, C2_TYPE_AABB, &m);

                if(m.count) {
                    ecs_set(it->world, it2.entities[j], Damage, { .amount = weapon[i].damage });
                    entity_despawn(it->entities[i]);
                }
            }
        }
    }
}