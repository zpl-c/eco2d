// debug tools written with Nuklear UI
#include "models/assets.h"
#include "world/blocks.h"
#include "models/items.h"

extern void tooltip_show(const char* name, float xpos, float ypos);

void ToolAssetInspector(void) {
	if (nk_begin(dev_ui, "Asset Inspector", nk_rect(400, 100, 240, 800),
	                                 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE| NK_WINDOW_TITLE))
	{
		for (int i = 0; i < MAX_ASSETS; i++) {
			uint16_t idx = assets_find(i);
			if (idx != ASSET_INVALID && nk_tree_push_id(dev_ui, NK_TREE_TAB, asset_names[i], NK_MINIMIZED, i)) {
				{
					// draw kind
					const char *asset_kind_name = assets_get_kind_name(idx);
					nk_labelf(dev_ui, NK_TEXT_LEFT, "kind: %s", asset_kind_name);
					nk_labelf(dev_ui, NK_TEXT_LEFT, "spawnable entity: %s", entity_spawn_provided(i) ? "true" : "false");

					if (nk_button_label(dev_ui, "show tooltip")) {
						Vector2 mpos = GetMousePosition();
						tooltip_show(asset_names[i] , mpos.x + 5, mpos.y + 5);
					}

					// draw block
					block_id blk_id = blocks_find(i);
					if (blk_id != 0xF) {
						if (nk_tree_push_id(dev_ui, NK_TREE_NODE, "block", NK_MINIMIZED, i)) {
							{
								nk_labelf(dev_ui, NK_TEXT_LEFT, "symbol: %s", zpl_bprintf("%c", blocks_get_symbol(blk_id)));
								nk_labelf(dev_ui, NK_TEXT_LEFT, "flags: %u", blocks_get_flags(blk_id));
								nk_labelf(dev_ui, NK_TEXT_LEFT, "drag: %f", blocks_get_drag(blk_id));
								nk_labelf(dev_ui, NK_TEXT_LEFT, "friction: %f", blocks_get_friction(blk_id));
								nk_labelf(dev_ui, NK_TEXT_LEFT, "bounce: %f", blocks_get_bounce(blk_id));
								nk_labelf(dev_ui, NK_TEXT_LEFT, "velx: %f", blocks_get_velx(blk_id));
								nk_labelf(dev_ui, NK_TEXT_LEFT, "vely: %f", blocks_get_vely(blk_id));
							}
							nk_tree_pop(dev_ui);
						}
					}

					// draw item
					item_id it_id = item_find_no_proxy(i);
					if (it_id != ASSET_INVALID) {
						if (nk_tree_push_id(dev_ui, NK_TREE_NODE, "item", NK_MINIMIZED, i)) {
							{
								item_desc it = item_get_desc(it_id);

								if (nk_button_label(dev_ui, "spawn")) {
									ecs_entity_t e = item_spawn(i, it.max_quantity);
									ecs_entity_t plr = camera_get().ent_id;

									Position const* origin = ecs_get(world_ecs(), plr, Position);
									entity_set_position(e, origin->x, origin->y);
								}

								nk_labelf(dev_ui, NK_TEXT_LEFT, "usage: %d", it.usage);
								nk_labelf(dev_ui, NK_TEXT_LEFT, "attachment: %d", it.attachment);
								nk_labelf(dev_ui, NK_TEXT_LEFT, "max quantity: %d", it.max_quantity);
								nk_labelf(dev_ui, NK_TEXT_LEFT, "has storage: %s", it.has_storage ? "true" : "false");

								// todo: draw item-specific data
							}
							nk_tree_pop(dev_ui);
						}
					}
				}
				nk_tree_pop(dev_ui);
			}
		}
		nk_end(dev_ui);
	}
}


void ToolEntityInspector(void) {
	if (nk_begin(dev_ui, "Entity Inspector", nk_rect(660, 100, 240, 800),
	             NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE| NK_WINDOW_TITLE))
	{
		static ecs_query_t *q = 0;
		if (!q) {
			q = ecs_query(world_ecs(), {
				.filter.terms = {
					{ .id = ecs_id(Position) },
					{ .id = ecs_id(Classify), .inout = EcsIn }
				}
			});
		}

		ecs_iter_t it = ecs_query_iter(world_ecs(), q);
		while (ecs_query_next(&it)) {
			Position *p = ecs_field(&it, Position, 1);
			const Classify *c = ecs_field(&it, Classify, 2);

			for (int i = 0; i < it.count; i++) {
				if (nk_tree_push_id(dev_ui, NK_TREE_NODE, zpl_bprintf("%lld [%s]", it.entities[i], class_names[c[i].id]), NK_MINIMIZED, (int)it.entities[i])) {
					{
						nk_label(dev_ui, "position:", NK_TEXT_LEFT);
						nk_property_float(dev_ui, "#x:", ZPL_F32_MIN, &p[i].x, ZPL_F32_MAX, 0.1f, 0.2f);
						nk_property_float(dev_ui, "#y:", ZPL_F32_MIN, &p[i].y, ZPL_F32_MAX, 0.1f, 0.2f);

						if (nk_button_label(dev_ui, "teleport to")) {
							ecs_entity_t plr = camera_get().ent_id;

							Position const* origin = ecs_get(world_ecs(), it.entities[i], Position);
							entity_set_position(plr, origin->x, origin->y);
						}

						if (nk_button_label(dev_ui, "teleport here")) {
							ecs_entity_t plr = camera_get().ent_id;

							Position const* origin = ecs_get(world_ecs(), plr, Position);
							entity_set_position(it.entities[i], origin->x, origin->y);
						}
					}
					nk_tree_pop(dev_ui);
				}
			}
		}

		nk_end(dev_ui);
	}
}