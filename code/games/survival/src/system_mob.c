#define MOB_SPAWN_DELAY (uint16_t)(20*1.5f)
#define MOB_SPAWN_PERCENTAGE_FROM_MAX 0.05f
#define MOB_INITIAL_MAX 50
#define MOB_GROWTH_FACTOR 5.0f
#define MOB_GROWTH_CONTROL 50.0f
#define MOB_SPAWN_DIST 12*WORLD_BLOCK_SIZE;
#define MOB_MAX_SPAWN_TRIES 5

static uint64_t max_mobs = MOB_INITIAL_MAX;
static uint64_t mob_kills = 0;
static uint16_t mob_spawn_timer = MOB_SPAWN_DELAY;
static int16_t player_spawn_counter = -1;

void recalc_max_mobs() {
	max_mobs = (uint64_t)zpl_round(MOB_INITIAL_MAX + MOB_GROWTH_FACTOR * zpl_exp((float)mob_kills / MOB_GROWTH_CONTROL));
	zpl_printf("Max mobs: %d\n", max_mobs);
}

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

#define MOB_MELEE_DIST 8000.0f
#define MOB_MELEE_DMG 8.5f
#define MOB_ATK_DELAY 10 
#define MOB_DESPAWN_TIMER 20*60*5

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
			Health *health = ecs_get_mut(it->world, m->plr, Health);
			health->dmg += MOB_MELEE_DMG;
			mob[i].atk_delay = MOB_ATK_DELAY;
		}
	}
}

void MobOnDead(ecs_iter_t *it) {
	Mob *mob = ecs_field(it, Mob, 1);
	Sprite *spr = ecs_field(it, Sprite, 2);
	Velocity *v = ecs_field(it, Velocity, 3);

	for (int i = 0; i < it->count; i++) {
		mob[i].despawn_timer = MOB_DESPAWN_TIMER;
		spr[i].frame = 3 + (rand()%5);
		v[i] = (Velocity){0.0f, 0.0f};
		ecs_remove(it->world, it->entities[i], PhysicsBody);

		++mob_kills;
		recalc_max_mobs();

		pkt_code_send(0, 0, (pkt_send_code){
			.code = SURV_CODE_SHOW_NOTIF,
			.data = "mob died"
		});
	}
}

void MobDespawnDead(ecs_iter_t *it) {
	Mob *mob = ecs_field(it, Mob, 1);

	for (int i = 0; i < it->count; i++) {
		if (mob[i].despawn_timer > 0) {
			TICK_VAR(mob[i].despawn_timer);
			continue;
		}

		entity_despawn(it->entities[i]);
	}
}

void MobSpawner(ecs_iter_t *it) {
	Position *pos = ecs_field(it, Position, 2);

	if (mob_spawn_timer > 0) {
		TICK_VAR(mob_spawn_timer);
		return;
	}

	ecs_iter_t mob_it = ecs_query_iter(it->world, ecs_mobpos_query);
	uint64_t curr_mobs = 0;
	while (ecs_query_next(&mob_it)) {
		curr_mobs += mob_it.count;
	}
	if (curr_mobs >= max_mobs) {
		return;
	}

	uint16_t mobs_to_spawn = zpl_max(1, (uint16_t)zpl_floor(MOB_SPAWN_PERCENTAGE_FROM_MAX*max_mobs));
	player_spawn_counter = (player_spawn_counter+1)%it->count;

	for (int i = player_spawn_counter; i < it->count; i++) {
		const uint32_t radius = MOB_SPAWN_DIST;
		uint32_t ox = (uint32_t)pos[i].x;
		uint32_t oy = (uint32_t)pos[i].y;

		uint8_t tries_done = 0;

		for (;;) {
			if (mobs_to_spawn == 0) {
				mob_spawn_timer = MOB_SPAWN_DELAY;
				return;
			}

			if (tries_done == MOB_MAX_SPAWN_TRIES) {
				break;
			}

			float angle = get_rand_between(0, 2*ZPL_PI);
			float dist = get_rand_between((float)radius, radius*1.5f);

			uint32_t cx = (uint32_t)(ox + dist * zpl_cos(angle));
	        uint32_t cy = (uint32_t)(oy + dist * zpl_sin(angle));

	        if (cx >= world_dim() || cy >= world_dim() || cx <= 0 || cy <= 0) {
	        	tries_done++;
	            continue;
	        }

	        world_block_lookup l = world_block_from_realpos((float)cx, (float)cy);
	        uint32_t flags = blocks_get_flags(l.bid);

	        if (flags & BLOCK_FLAG_COLLISION) {
	        	tries_done++;
	            continue;
	        }

	        ecs_entity_t e = entity_spawn_id(ASSET_MOB);
			entity_set_position(e, (float)cx, (float)cy);
			ecs_add(world_ecs(), e, MobMelee);
			mobs_to_spawn--;
		}
	}
}
