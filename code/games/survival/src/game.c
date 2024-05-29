#include "core/game.h"
#include "game.h"
#include "world/world.h"
#include "models/components.h"
#include "systems/systems.h"
#include "models/entity.h"
#include "world/entity_view.h"

#include "gui/notifications.h"

float get_rand_between(float min, float max) {
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

static ecs_query_t *ecs_mobpos_query = NULL;
static ecs_query_t *ecs_pawn_query = NULL;

// custom systems
#include "system_mob.c"
#include "system_weapon.c"

#define PLAYER_RESPAWN_BLAST_FORCE 1200.0f

void PlayerRespawn(ecs_iter_t *it) {
	Respawn *r = ecs_field(it, Respawn, 1);
	Input *in = ecs_field(it, Input, 2);
	Sprite *s = ecs_field(it, Sprite, 3);
	Position *p = ecs_field(it, Position, 4);
	Health *h = ecs_field(it, Health, 5);

	for (int i = 0; i < it->count; i++) {
		if (r[i].timer > 0) {
			TICK_VAR(r[i].timer);
			continue;
		}

		ecs_remove(it->world, it->entities[i], Respawn);
		ecs_remove(it->world, it->entities[i], Dead);
		in[i].is_blocked = 0;
		s[i].spritesheet = 0;
		h[i].hp = h[i].max_hp;

		size_t ents_count;
        int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);
        
        for (size_t j = 0; j < ents_count; j++) {
            uint64_t ent_id = ents[j];

            if (!ecs_get(it->world, ent_id, Mob) || ecs_get(it->world, ent_id, Dead)) {
            	continue;
            }

            const Position *p2 = ecs_get(it->world, ent_id, Position);
            Velocity *v = ecs_get_mut(it->world, ent_id, Velocity);
            float dx = p2->x - p[i].x;
            float dy = p2->y - p[i].y;
            float range = zpl_sqrt(dx*dx + dy*dy);
            if (range <= 5*WORLD_BLOCK_SIZE) {
            	Health *hp = ecs_get_mut(it->world, ent_id, Health);
            	hp->dmg += hp->max_hp/2.0f;

            	v->x += (dx/range)*PLAYER_RESPAWN_BLAST_FORCE;
				v->y += (dy/range)*PLAYER_RESPAWN_BLAST_FORCE;
            }
        }		
	}
}

void mob_systems(ecs_world_t *ecs) {
	ECS_SYSTEM_TICKED_EX(ecs, MobDetectPlayers, EcsPostUpdate, 100.0f, components.Position, components.Mob, !components.Dead);
	ECS_SYSTEM(ecs, MobMovement, EcsPostUpdate, components.Velocity, components.Position, components.MobHuntPlayer, !components.Dead);
	ECS_SYSTEM_TICKED(ecs, MobMeleeAtk, EcsPostUpdate, components.Position, components.Mob, components.MobHuntPlayer, components.MobMelee, !components.Dead);
	ECS_SYSTEM_TICKED(ecs, MobDespawnDead, EcsPostUpdate, components.Mob, components.Dead);
	ECS_SYSTEM_TICKED(ecs, MobSpawner, EcsPostUpdate, components.Input, components.Position, !components.Dead);
	ECS_SYSTEM_TICKED(ecs, PlayerRespawn, EcsPostUpdate, components.Respawn, components.Input, components.Sprite, components.Position, components.Health);

	//NOTE(DavoSK): weapons
	ecs_mobpos_query = ecs_query_new(world_ecs(), "components.Mob, components.Position, components.Health, components.Velocity, !components.Dead");
	ecs_pawn_query = ecs_query_new(world_ecs(), "components.Position, components.Health, components.Velocity, !components.Dead");
	ECS_SYSTEM_TICKED(ecs, WeaponKnifeMechanic, EcsPostUpdate, components.WeaponKnife, components.Position, components.Input, !components.Dead);
	ECS_SYSTEM_TICKED(ecs, WeaponProjectileHit, EcsPostUpdate, components.WeaponProjectile, components.Position, components.Rotation);
	ECS_SYSTEM_TICKED(ecs, WeaponProjectileExpire, EcsPostUpdate, components.WeaponProjectile, components.Position);
	ECS_OBSERVER(ecs, MobOnDead, EcsOnAdd, components.Mob, components.Sprite, components.Velocity, components.Dead);
}

void game_init(bool new_db) {

}

void game_input() {
	game_core_input();
}

void game_update() {
	game_core_update();
}

void game_render() {
	game_core_render();
}

void game_init_ecs() {
	mob_systems(world_ecs());
}

void game_player_departed(uint64_t ent) {

}

void game_player_joined(uint64_t ent) {
	notification_push("test1", "Hello World!");

	//NOTE(DavoSK): add weapon component for testing
	ecs_world_t* world = world_ecs();
	ecs_set(world, (ecs_entity_t)ent, WeaponKnife, {
		.projectile_count = 10,
		.damage = 10,
		.spawn_delay = WEAPON_KNIFE_SPAWN_DELAY
	});
}

void game_player_died(uint64_t ent) {
	Sprite *spr = ecs_get_mut(world_ecs(), ent, Sprite);
	Velocity *v = ecs_get_mut(world_ecs(), ent, Velocity);
	spr->frame = 3 + (rand()%5);
	spr->spritesheet = 69; /*special code*/
	*v = (Velocity){0.0f, 0.0f};
	ecs_remove(world_ecs(), ent, PhysicsBody);
	ecs_set(world_ecs(), ent, Respawn, { 100 });
}

void game_client_receive_code(pkt_send_code data) {
	switch (data.code) {
		case SURV_CODE_SHOW_NOTIF: {
			notification_push("TEST", data.data);
		} break;
	}
}
