#pragma once
#include "platform/system.h"
#include "models/assets.h"

#include "models/components.h"

// NOTE(zaklaus): resolves recipe dependencies and consumes reagents
// to enqueue a production of a new item.
// TODO(zaklaus): "items" is assumed to come from ItemContainer component.
asset_id craft_perform_recipe(ecs_entity_t *items, asset_id producer, asset_id target, uint32_t *quantity, int32_t *process_ticks);

// NOTE(zaklaus): mostly used by item router so we don't push reagents out
bool craft_is_reagent_used_in_producer(asset_id reagent, asset_id producer);

// used to filter out craftables
bool craft_is_item_produced_by_producer(asset_id item, asset_id producer);

// NOTE(zaklaus): utilities
uint16_t craft_get_num_recipes(void);
asset_id craft_get_recipe_asset(uint16_t id);

//~TODO(zaklaus): not implemented and might get removed

// NOTE(zaklaus): informs us on whether this product has any byproducts desired.
asset_id craft_has_byproducts(asset_id product);

// NOTE(zaklaus): resolves the production chain and analyses the amount of items required
// and a number of hops (production layers) needed to produce the item.
// optionally, it allows to calculate "direct_cost" of the product.
uint32_t craft_resolve_graph(asset_id product, uint16_t *hops, uint8_t direct_cost);
