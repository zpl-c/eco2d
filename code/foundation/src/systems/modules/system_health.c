#define HAZARD_BLOCK_DMG 5.0f

void HurtOnHazardBlock(ecs_iter_t *it) {
	Position *p = ecs_field(it, Position, 1);
	Health *h = ecs_field(it, Health, 2);
    
	for (int i = 0; i < it->count; i++) {
		world_block_lookup l = world_block_from_realpos(p[i].x, p[i].y);
		if (blocks_get_flags(l.bid) & BLOCK_FLAG_HAZARD) {
			h[i].dmg += HAZARD_BLOCK_DMG;
		}
	}
}

//#define HP_REGEN_PAIN_COOLDOWN 5.0f

void RegenerateHP(ecs_iter_t *it) {
	Health *h = ecs_field(it, Health, 1);
	HealthRegen *r = ecs_field(it, HealthRegen, 2);
    
	for (int i = 0; i < it->count; i++) {
		// TODO delay regen on hurt
		if (h[i].hp < h[i].max_hp) {
			h[i].hp += r->amt;
			h[i].hp = zpl_min(h[i].max_hp, h[i].hp);
			entity_wake(it->entities[i]);
		}
	}
}

void ProcessHealthDamage(ecs_iter_t *it) {
	for (int i = 0; i < it->count; i++) {
		Health *hp = ecs_get_mut(it->world, it->entities[i], Health);
		if (hp->dmg > 0.0f) {
			hp->hp = zpl_max(hp->hp-hp->dmg, 0.0f);
			hp->dmg = 0.0f;
			ecs_set(it->world, it->entities[i], HealDelay, { .delay = 10 });

			if (hp->hp <= 0.0f) {
				ecs_add(it->world, it->entities[i], Dead);
			}
		}
	}
}

void OnDead(ecs_iter_t *it) {
	for (int i = 0; i < it->count; i++) {
		const ClientInfo *ci = ecs_get(it->world, it->entities[i], ClientInfo);
		Input *pi = ecs_get_mut_if(it->world, it->entities[i], Input);

		if (ci) {
			pkt_notification_send(0, 0, "Someone died!", zpl_bprintf("Player %d has died!", it->entities[i]));
			game_player_died(it->entities[i]);
		}

		if (pi) {
			*pi = (Input) { 0 };
			pi->is_blocked = 1;
		}
	}
}

void TickDownHealDelay(ecs_iter_t *it) {
	HealDelay *h = ecs_field(it, HealDelay, 1);
    
	for (int i = 0; i < it->count; i++) {
		TICK_VAR(h[i].delay);

		if (h[i].delay == 0) {
			ecs_remove(it->world, it->entities[i], HealDelay);
		}
	}
}
