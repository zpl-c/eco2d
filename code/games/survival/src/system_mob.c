void MobDetectPlayers(ecs_iter_t *it) {
	Position *p = ecs_field(it, Position, 1);

	for (int i = 0; i < it->count; i++) {
		float closest_ent_dist = ZPL_F32_MAX;
		uint64_t closest_ent = 0;

		ecs_iter_t pit = ecs_query_iter(world_ecs(), world_ecs_player());

		while (ecs_query_next(&pit)) {
			Position *p2 = ecs_field(&pit, Position, 2);

			for (int j = 0; j < pit.count; j++) {
				float dx = p2->x - p[j].x;
				float dy = p2->y - p[j].y;
				float range = (dx*dx + dy*dy);

				if (range < closest_ent_dist)
					closest_ent = pit.entities[j];
			}
		}

		if (!closest_ent) 
			continue;

		ecs_set(it->world, it->entities[i], MobHuntPlayer, { .plr = closest_ent });
	}
}

void MobDetectPlayers1(ecs_iter_t *it) {
	
}

#define MOB_MOVEMENT_SPEED 300.0f

void MobMovement(ecs_iter_t *it) {
	Velocity *v = ecs_field(it, Velocity, 1);
	Position *p = ecs_field(it, Position, 2);
	MobHuntPlayer *m = ecs_field(it, MobHuntPlayer, 3);

	for (int i = 0; i < it->count; i++) {
		const Position *p2 = ecs_get(it->world, m->plr, Position);
		zpl_vec2 pos1 = { .x = p[i].x, .y = p[i].y };
		zpl_vec2 pos2 = { .x = p2->x, .y = p2->y };
		zpl_vec2 dir;
		zpl_vec2_sub(&dir, pos2, pos1);
		zpl_vec2_norm(&dir, dir);

		v[i].x += dir.x*(MOB_MOVEMENT_SPEED*safe_dt(it));
		v[i].y += dir.y*(MOB_MOVEMENT_SPEED*safe_dt(it));
 
		entity_wake(it->entities[i]);
	}
}

#define MOB_MELEE_DIST 4000.0f
#define MOB_MELEE_DMG 1.5f
#define MOB_ATK_DELAY 10 

void MobMeleeAtk(ecs_iter_t *it) {
	Position *p = ecs_field(it, Position, 1);
	Mob *mob = ecs_field(it, Mob, 2);
	MobHuntPlayer *m = ecs_field(it, MobHuntPlayer, 3);

	for (int i = 0; i < it->count; i++) {
		if (mob[i].atk_delay > 0) {
			TICK_VAR(mob[i].atk_delay);
			continue;
		}

		const Position *p2 = ecs_get(it->world, m->plr, Position);
		float dx = p2->x - p[i].x;
		float dy = p2->y - p[i].y;
		float range = (dx*dx + dy*dy);

		if (range < MOB_MELEE_DIST) {
			Health *health = ecs_get_mut_ex(it->world, m->plr, Health);
			health->dmg += MOB_MELEE_DMG;
		}
		mob[i].atk_delay = MOB_ATK_DELAY;
	}
}

void MobOnDead(ecs_iter_t *it) {
	for (int i = 0; i < it->count; i++) {
		entity_despawn(it->entities[i]);
	}
}