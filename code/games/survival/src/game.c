#include "core/game.h"
#include "game.h"
#include "world/world.h"
#include "models/components.h"
#include "systems/systems.h"
#include "models/entity.h"
#include "world/entity_view.h"

#include "gui/notifications.h"

// custom systems
#include "system_mob.c"
#include "system_weapon.c"

void mob_systems(ecs_world_t *ecs) {
	ECS_SYSTEM_TICKED_EX(ecs, MobDetectPlayers, EcsPostUpdate, 100.0f, components.Position, components.Mob);
	ECS_SYSTEM(ecs, MobMovement, EcsPostUpdate, components.Velocity, components.Position, components.MobHuntPlayer);
	ECS_SYSTEM_TICKED(ecs, MobMeleeAtk, EcsPostUpdate, components.Position, components.Mob, components.MobHuntPlayer, components.MobMelee);
	
	//NOTE(DavoSK): weapons
	ECS_SYSTEM_TICKED(ecs, WeaponKnifeMechanic, EcsPostUpdate, components.WeaponKnife, components.Position, components.Input);
	ECS_SYSTEM_TICKED(ecs, WeaponProjectileExpire, EcsPostUpdate, components.WeaponProjectile, components.Position);
	//ECS_OBSERVER(ecs, MobDetectPlayers1, EcsOnAdd, components.Mob);
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
		.projectile_count = 1,
		.damage = 10,
		.spawn_delay = WEAPON_KNIFE_SPAWN_DELAY
	});
}

void game_player_departed(uint64_t ent) {

}