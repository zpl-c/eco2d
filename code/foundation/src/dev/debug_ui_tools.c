// debug tools written with Nuklear UI
#include "models/assets.h"
#include "world/blocks.h"
#include "models/items.h"

void ToolAssetInspector(void) {
	if (nk_begin(nk_ctx, "Asset Inspector", nk_rect(400, 100, 240, 800),
	                                 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE| NK_WINDOW_TITLE))
	{
		for (int i = 0; i < MAX_ASSETS; i++) {
			uint16_t idx = assets_find(i);
			if (idx != ASSET_INVALID && nk_tree_push_id(nk_ctx, NK_TREE_TAB, asset_names[i], NK_MINIMIZED, i)) {
				{
					// draw kind
					const char *asset_kind_name = assets_get_kind_name(idx);
					nk_labelf(nk_ctx, NK_TEXT_LEFT, "kind: %s", asset_kind_name);
					nk_labelf(nk_ctx, NK_TEXT_LEFT, "spawnable entity: %s", entity_spawn_provided(i) ? "true" : "false");

					// draw block
					block_id blk_id = blocks_find(i);
					if (blk_id != 0xF) {
						if (nk_tree_push_id(nk_ctx, NK_TREE_NODE, "block", NK_MINIMIZED, i)) {
							{
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "symbol: %s", zpl_bprintf("%c", blocks_get_symbol(blk_id)));
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "flags: %u", blocks_get_flags(blk_id));
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "drag: %f", blocks_get_drag(blk_id));
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "friction: %f", blocks_get_friction(blk_id));
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "bounce: %f", blocks_get_bounce(blk_id));
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "velx: %f", blocks_get_velx(blk_id));
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "vely: %f", blocks_get_vely(blk_id));
							}
							nk_tree_pop(nk_ctx);
						}
					}

					// draw item
					item_id it_id = item_find_no_proxy(i);
					if (it_id != ASSET_INVALID) {
						if (nk_tree_push_id(nk_ctx, NK_TREE_NODE, "item", NK_MINIMIZED, i)) {
							{
								item_desc it = item_get_desc(it_id);

								if (nk_button_label(nk_ctx, "spawn")) {
									ecs_entity_t e = item_spawn(i, 1);
									ecs_entity_t plr = camera_get().ent_id;

									Position const* origin = ecs_get(world_ecs(), plr, Position);
									entity_set_position(e, origin->x, origin->y);
								}

								nk_labelf(nk_ctx, NK_TEXT_LEFT, "usage: %d", it.usage);
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "attachment: %d", it.attachment);
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "max quantity: %d", it.max_quantity);
								nk_labelf(nk_ctx, NK_TEXT_LEFT, "has storage: %s", it.has_storage ? "true" : "false");

								// todo: draw item-specific data
							}
							nk_tree_pop(nk_ctx);
						}
					}
				}
				nk_tree_pop(nk_ctx);
			}
		}
		nk_end(nk_ctx);
	}
}
