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

void WeaponProjectileHit(ecs_iter_t *it) {
    //TODO(DavoSK): hit entity, attach take damage component
    //remove projectile
}