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

void mob_systems(ecs_world_t *ecs) {
	ECS_SYSTEM_TICKED_EX(ecs, MobDetectPlayers, EcsPostUpdate, 100.0f, components.Position, components.Mob, !components.Dead);
	ECS_SYSTEM(ecs, MobMovement, EcsPostUpdate, components.Velocity, components.Position, components.MobHuntPlayer, !components.Dead);
	ECS_SYSTEM_TICKED(ecs, MobMeleeAtk, EcsPostUpdate, components.Position, components.Mob, components.MobHuntPlayer, components.MobMelee, !components.Dead);
	ECS_SYSTEM_TICKED(ecs, MobDespawnDead, EcsPostUpdate, components.Mob, components.Dead);
	ECS_SYSTEM_TICKED(ecs, MobSpawner, EcsPostUpdate, components.Input, components.Position);

	//NOTE(DavoSK): weapons
	ecs_mobpos_query = ecs_query_new(world_ecs(), "components.Mob, components.Position, components.Health, components.Velocity, !components.Dead");
	ecs_pawn_query = ecs_query_new(world_ecs(), "components.Position, components.Health, components.Velocity, !components.Dead");
	ECS_SYSTEM_TICKED(ecs, WeaponKnifeMechanic, EcsPostUpdate, components.WeaponKnife, components.Position, components.Input, !components.Dead);
	ECS_SYSTEM_TICKED(ecs, WeaponProjectileHit, EcsPostUpdate, components.WeaponProjectile, components.Position, components.Rotation);
	ECS_SYSTEM_TICKED(ecs, WeaponProjectileExpire, EcsPostUpdate, components.WeaponProjectile, components.Position);
	ECS_OBSERVER(ecs, MobOnDead, EcsOnAdd, components.Mob, components.Sprite, components.Velocity, components.Dead);
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

void game_setup_ecs() {
	mob_systems(world_ecs());
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

void game_player_departed(uint64_t ent) {

}

void game_client_receive_code(pkt_send_code data) {
	switch (data.code) {
		case SURV_CODE_SHOW_NOTIF: {
			notification_push("TEST", data.data);
		} break;
	}
}
