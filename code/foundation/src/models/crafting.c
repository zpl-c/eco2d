#include "crafting.h"
#include "models/items.h"
#include "models/database.h"

//#include "lists/crafting_list.c"
static recipe *recipes;
#define MAX_RECIPES (zpl_array_count(recipes))

void craft_db(void) {
    zpl_array_init(recipes, zpl_heap());
    db_push("SELECT * FROM recipes;");
    for (size_t i=0, end=db_rows(); i<end; i++) {
        recipe r={0};
        r.product = db_int("product", i);
        r.product_qty = db_int("product_qty", i);
        r.process_ticks = db_int("process_ticks", i);
        r.producer = db_int("producer", i);
        zpl_array_init(r.reagents, zpl_heap());
        db_push(zpl_bprintf("SELECT * FROM recipe_reagents WHERE recipe_id=%d;", i+1));
        for (size_t j=0, j_end=db_rows(); j<j_end; j++) {
            reagent rea={0};
            size_t reagent_id = db_int("reagent_id", j);
            db_push(zpl_bprintf("SELECT * FROM reagents WHERE id=%d;", reagent_id));
                rea.id = db_int("asset_id", 0);
                rea.qty = db_int("qty", 0);
            db_pop();
            zpl_array_append(r.reagents, rea);
        }
        db_pop();
        zpl_array_append(recipes, r);
    }
    db_pop();
}

uint32_t craft__find_num_recipes_by_reagent(asset_id producer, asset_id id) {
    uint32_t num_recipes=0;
    for (int i = 0; i < (int)MAX_RECIPES; ++i) {
        if (recipes[i].producer == producer) {
            for (int j = 0; recipes[i].reagents[j].id; ++j) {
                if (recipes[i].reagents[j].id == id) {
                    ++num_recipes;
                    break;
                }
            }
        }
    }
    return num_recipes;
}

recipe *craft__find_recipe_by_reagent(asset_id producer, asset_id id, uint32_t slot_id) {
    for (int i = 0; i < (int)MAX_RECIPES; ++i) {
        if (recipes[i].producer == producer) {
            for (int j = 0; recipes[i].reagents[j].id; ++j) {
                if (recipes[i].reagents[j].id == id) {
                    if (slot_id > 0) {
                        --slot_id;
                        continue;
                    }
                    return &recipes[i];
                }
            }
        }
    }
    return NULL;
} 

uint16_t craft_get_recipe_id_from_product(asset_id id) {
	for (int i = 0; i < (int)MAX_RECIPES; ++i) {
		if (id == recipes[i].product) {
			return i;
		}
	}
	return ASSET_INVALID;
}

recipe craft_get_recipe_data(uint16_t i) {
	ZPL_ASSERT(i < MAX_RECIPES);
	return recipes[i];
}

bool craft_is_reagent_used_in_producer(asset_id reagent, asset_id producer) {
    return craft__find_num_recipes_by_reagent(producer, reagent) > 0;
}

bool craft_is_item_produced_by_producer(asset_id item, asset_id producer) {
	for (int i = 0; i < (int)MAX_RECIPES; ++i) {
		if (recipes[i].producer == producer && item == recipes[i].product) {
			return true;
		}
	}
	return false;
}

bool craft_is_item_produced_by_reagent(asset_id item, asset_id reagent) {
	for (int i = 0; i < (int)MAX_RECIPES; ++i) {
		if (item == recipes[i].product) {
			for (int j = 0; recipes[i].reagents[j].id; ++j) {
				if (recipes[i].reagents[j].id == reagent)
					return true;
			}
		}
	}
	return false;
}

uint16_t craft_get_num_recipes(void) {
    return MAX_RECIPES;
}

asset_id craft_get_recipe_asset(uint16_t id) {
    ZPL_ASSERT(id < MAX_RECIPES);
    return recipes[id].product;
}

asset_id craft_perform_recipe(ecs_entity_t *items, asset_id producer, asset_id target, uint32_t *quantity, int32_t *process_ticks) {
    ZPL_ASSERT_NOT_NULL(items);
    
    for (int i = 0; i < ITEMS_CONTAINER_SIZE; i++) {
        ecs_entity_t item_slot_ent = items[i];
        if (item_slot_ent == 0) continue;
        Item *item = item_get_data(item_slot_ent);
        if (!item) continue;
        
        uint32_t num_recipes = craft__find_num_recipes_by_reagent(producer, item->kind);
        for (uint32_t rec_i = 0; rec_i < num_recipes; ++rec_i) {
            // TODO(zaklaus): slow, find a better way to retrieve known recipes
            recipe *rec = craft__find_recipe_by_reagent(producer, item->kind, rec_i);
            if (!rec) {
                // NOTE(zaklaus): this item is not used as a reagent, skip it.
                // TODO(zaklaus): is this a bug? should we assert?
                continue;
            }
            
            if (target != 0 && rec->product != target) {
                // NOTE(zaklaus): we were asked to produce a specific product, 
                // however this recipe is not compatible, bail.
                continue;
            }
            
            uint8_t skip_slot=0;
            
            // NOTE(zaklaus): analyse if all the reagents are present
            for (int j = 0; rec->reagents[j].id; ++j) {
                reagent *rea = &rec->reagents[j];
                uint32_t pending_qty = rea->qty;
                
                for (int k = 0; k < ITEMS_CONTAINER_SIZE; k++) {
                    ecs_entity_t rea_item_slot_ent = items[k];
                    if (rea_item_slot_ent == 0) continue;
                    Item *rea_item = item_get_data(rea_item_slot_ent);
                    if (!rea_item) continue;
                    
                    if (rea->id == rea_item->kind && rea_item->quantity > 0) {
                        pending_qty -= zpl_min(pending_qty, rea_item->quantity);
                        
                        if (pending_qty == 0) {
                            break;
                        }
                    }
                }
                
                if (pending_qty > 0) {
                    // NOTE(zaklaus): reagent not found, bail
                    skip_slot=1;
                    break;
                }
            }
            
            // NOTE(zaklaus): demand not met, bye!
            if (skip_slot)
                continue;
            
            // NOTE(zaklaus): deplete used reagents
            for (int j = 0; rec->reagents[j].id; ++j) {
                reagent *rea = &rec->reagents[j];
                uint32_t pending_qty = rea->qty;
                
                for (int k = 0; k < ITEMS_CONTAINER_SIZE; k++) {
                    ecs_entity_t rea_item_slot_ent = items[k];
                    if (rea_item_slot_ent == 0) continue;
                    Item *rea_item = item_get_data(rea_item_slot_ent);
                    if (!rea_item) continue;
                    
                    if (rea->id == rea_item->kind && rea_item->quantity > 0) {
                        rea_item->quantity -= zpl_min(pending_qty, rea_item->quantity);
                        pending_qty -= zpl_min(pending_qty, rea_item->quantity);
                        if (rea_item->quantity == 0) {
                            item_despawn(rea_item_slot_ent);
                            items[k] = 0;
                        }
                        
                        if (pending_qty == 0) {
                            break;
                        }
                    }
                }
            }
            
            // NOTE(zaklaus): all done, return the product and its qty
            *quantity = rec->product_qty;
			*process_ticks = rec->process_ticks;
            return rec->product;
        }
    }
    return 0;
}

// TODO(zaklaus): might be removed
asset_id craft_has_byproducts(asset_id product) {
    return 0xFF;
}

// TODO(zaklaus): might be removed
uint32_t craft_resolve_graph(asset_id product, uint16_t *hops, uint8_t direct_cost) {
    return 0;
}
