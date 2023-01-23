#include "creature.h"
#include "world/world.h"

#include "models/entity.h"
#include "world/entity_view.h"
#include "models/components.h"

uint64_t creature_spawn(void) {
	ecs_entity_t e = entity_spawn(EKIND_DEMO_NPC);

	Creature *c = ecs_get_mut_ex(world_ecs(), e, Creature);
	c->hunger_satisfied = 0;
	c->mating_satisfied = rand() % 1800;
	c->life_remaining = 500 + rand() % 5200;

	return (uint64_t)e;
}

void creature_despawn(uint64_t ent_id) {
	entity_despawn(ent_id);
}
