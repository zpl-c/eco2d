#define WEAPON_KNIFE_SPAWN_DELAY 20
#define WEAPON_PROJECTILE_POS_OFFSET 0.2f
#define WEAPON_PROJECTILE_SPEED 10.0f

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
            ecs_entity_t projectile_entity = ecs_new_entity(it->world, "WeaponProjectile");
            ecs_set(it->world, projectile_entity, Position, { 
                .x=pos[i].x, 
                .y=pos[i].y+get_rand_between(-WEAPON_PROJECTILE_POS_OFFSET, WEAPON_PROJECTILE_POS_OFFSET)
            });
            
            ecs_set(it->world, projectile_entity, Velocity, {
                .x=input[i].x*WEAPON_PROJECTILE_SPEED,
                .y=input[i].y*WEAPON_PROJECTILE_SPEED
            });
        }

		weapon[i].spawn_delay = WEAPON_KNIFE_SPAWN_DELAY;
	}
}