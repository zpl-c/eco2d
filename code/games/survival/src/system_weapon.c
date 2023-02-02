#define WEAPON_KNIFE_SPAWN_DELAY 20
#define WEAPON_PROJECTILE_POS_OFFSET 3.2f
#define WEAPON_PROJECTILE_SPEED 10.0f
#define WEAPON_PROJECTILE_RANGE_LIFETIME 10.0f

//TODO(DavoSK): move to helpers, add srand
float get_rand_between(float min, float max) {
    float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * ( max - min );      /* [min, max] */
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
            ecs_set(it->world, e, WeaponProjectile, { .damage=weapon[i].damage });
            ecs_set(it->world, e, Sprite, { .spritesheet=0, .frame=2347 });
            ecs_set(it->world, e, InAir, {0});

            ecs_set(it->world, e, Velocity, {
                .x=input[i].x*WEAPON_PROJECTILE_SPEED,
                .y=input[i].y*WEAPON_PROJECTILE_SPEED
            });

            Position *dest = ecs_get_mut(world_ecs(), e, Position);
            dest->x = pos[i].x;
            dest->y = pos[i].y+get_rand_between(-WEAPON_PROJECTILE_POS_OFFSET, WEAPON_PROJECTILE_POS_OFFSET);
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
        const float d = zpl_vec2_mag(v_origin, v_pos);
        
        if(d > WEAPON_PROJECTILE_RANGE_LIFETIME) {
            entity_despawn(it->entities[i]);
        }
    }
}

void WeaponProjectileHit(ecs_iter_t *it) {

}