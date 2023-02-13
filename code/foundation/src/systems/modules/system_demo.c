#include <math.h>

void DemoNPCMoveAround(ecs_iter_t *it) {
    Velocity *v = ecs_field(it, Velocity, 1);
    for (int i = 0; i < it->count; i++) {
        float d = zpl_quake_rsqrt(v[i].x*v[i].x + v[i].y*v[i].y);
        v[i].x += (v[i].x*d*game_rules.demo_npc_move_speed*safe_dt(it) + zpl_cos(zpl_to_radians((float)(rand()%360)))*game_rules.demo_npc_steer_speed*safe_dt(it));
        v[i].y += (v[i].y*d*game_rules.demo_npc_move_speed*safe_dt(it) + zpl_sin(zpl_to_radians((float)(rand()%360)))*game_rules.demo_npc_steer_speed*safe_dt(it));

        entity_wake(it->entities[i]);
    }
}

//------------------------------------------------------------------------

#define CREATURE_FOOD_SATISFY_FOR 200
#define CREATURE_MATING_SATISFY_FOR 300
#define CREATURE_INTERACT_RANGE 5625.0f
#define CREATURE_SEEK_FOOD_MOVEMENT_SPEED 0.98f
#define CREATURE_SEEK_MATE_MOVEMENT_SPEED 0.357f
#define CREATURE_SEEK_ROAM_MOVEMENT_SPEED 50.0f // *dt

void CreatureCheckNeeds(ecs_iter_t *it) {
	Creature *c = ecs_field(it, Creature, 1);

	for (int i = 0; i < it->count; i++) {
		// check hunger
		if (c[i].hunger_satisfied < 1) {
			ecs_add(it->world, it->entities[i], SeeksFood);
		}

		// check mating
		if (c[i].mating_satisfied < 1) {
			ecs_add(it->world, it->entities[i], SeeksCompanion);
		}

		// die of an old age
		if (c[i].life_remaining < 1) {
			entity_despawn(it->entities[i]);
			continue;
		}

		// tick down needs
		TICK_VAR(c[i].hunger_satisfied);
		TICK_VAR(c[i].mating_satisfied);
		TICK_VAR(c[i].life_remaining);
	}
}

void CreatureSeekFood(ecs_iter_t *it) {
	Creature *c = ecs_field(it, Creature, 1);
	Position *p = ecs_field(it, Position, 2);
	Velocity *v = ecs_field(it, Velocity, 3);

	for (int i = 0; i < it->count; i++) {
		size_t ents_count;
		int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);

		float closest_ent_dist = ZPL_F32_MAX;
		uint64_t closest_ent = 0;
		Position *p2 = 0;

		// find the closest item of kind ASSET_CREATURE_FOOD
		for (size_t j = 0; j < ents_count; j++) {
			Item *drop = 0;
			uint64_t ent_id = ents[j];
			if ((drop = ecs_get_mut_if(it->world, ent_id, Item))) {
				if (drop->kind != ASSET_CREATURE_FOOD)
					continue;

				p2 = ecs_get_mut(it->world, ent_id, Position);
				float dx = p2->x - p[i].x;
				float dy = p2->y - p[i].y;
				float range = (dx*dx + dy*dy);

				// item is close enough, eat it!
				if (range < CREATURE_INTERACT_RANGE) {
					drop->quantity--;
                                
					if (drop->quantity == 0)
						item_despawn(ent_id);

					c[i].hunger_satisfied = CREATURE_FOOD_SATISFY_FOR;
					ecs_remove(it->world, it->entities[i], SeeksFood);
				}
				else if (range < closest_ent_dist)
					closest_ent = ent_id;
			}
		}

		// drift towards the item
		if (closest_ent) {
			float dx = p2->x - p[i].x;
			float dy = p2->y - p[i].y;
			float r = 1; //zpl_sqrt(dx*dx + dy*dy);
			v[i].x = (dx/r) * CREATURE_SEEK_MATE_MOVEMENT_SPEED;
			v[i].y = (dy/r) * CREATURE_SEEK_MATE_MOVEMENT_SPEED;
		} else {
			// die if no food is left
			entity_despawn(it->entities[i]);
			continue;
		}
	}
}

void CreatureSeekCompanion(ecs_iter_t *it) {
	Creature *c = ecs_field(it, Creature, 1);
	Position *p = ecs_field(it, Position, 2);
	Velocity *v = ecs_field(it, Velocity, 3);

	for (int i = 0; i < it->count; i++) {
		size_t ents_count;
		int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);

		float closest_ent_dist = ZPL_F32_MAX;
		uint64_t closest_ent = 0;
		Position *p2 = 0;

		// find the closest entity that also seeks a companion
		for (size_t j = 0; j < ents_count; j++) {
			uint64_t ent_id = ents[j];
			if (ent_id != it->entities[i] && ecs_get_if(it->world, ent_id, SeeksCompanion)) {
				p2 = ecs_get_mut(it->world, ent_id, Position);
				float dx = p2->x - p[i].x;
				float dy = p2->y - p[i].y;
				float range = (dx*dx + dy*dy);

				// creature is close enough, mate them!
				if (range < CREATURE_INTERACT_RANGE) {
					// remove the need
					c[i].mating_satisfied = CREATURE_MATING_SATISFY_FOR;
					ecs_remove(it->world, it->entities[i], SeeksCompanion);
					ecs_remove(it->world, ent_id, SeeksCompanion);

					// spawn a new creature
					uint64_t ch = entity_spawn_id(ASSET_CREATURE);
					entity_set_position(ch, p[i].x, p[i].y);
				}
				else if (range < closest_ent_dist)
					closest_ent = ent_id;
			}
		}

		// drift towards the creature
		if (closest_ent) {
			float dx = p2->x - p[i].x;
			float dy = p2->y - p[i].y;
			float r = 1; //zpl_sqrt(dx*dx + dy*dy);
			v[i].x = (dx/r) * CREATURE_SEEK_MATE_MOVEMENT_SPEED;
			v[i].y = (dy/r) * CREATURE_SEEK_MATE_MOVEMENT_SPEED;
			entity_wake(it->entities[i]);
		} else {
			// no companion is found, let's try again later.
			c[i].mating_satisfied = CREATURE_MATING_SATISFY_FOR;
			ecs_remove(it->world, it->entities[i], SeeksCompanion);
		}
	}
}

void CreatureRoamAround(ecs_iter_t *it) {
	Velocity *v = ecs_field(it, Velocity, 1);
	for (int i = 0; i < it->count; i++) {
		float d = zpl_quake_rsqrt(v[i].x*v[i].x + v[i].y*v[i].y);
		if (zpl_abs(v[i].x) < 0.1f)
			v[i].x = (float)(rand() % 5);
		if (zpl_abs(v[i].y) < 0.1f)
			v[i].y = (float)(rand() % 5);
		v[i].x += (v[i].x*d*CREATURE_SEEK_ROAM_MOVEMENT_SPEED*safe_dt(it) + zpl_cos(zpl_to_radians((float)(rand()%360)))*game_rules.demo_npc_steer_speed*safe_dt(it));
		v[i].y += (v[i].y*d*CREATURE_SEEK_ROAM_MOVEMENT_SPEED*safe_dt(it) + zpl_sin(zpl_to_radians((float)(rand()%360)))*game_rules.demo_npc_steer_speed*safe_dt(it));

		entity_wake(it->entities[i]);
	}
}