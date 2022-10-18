#include "crafting.h"
#include "models/items.h"

typedef struct {
    asset_id id;
    uint32_t qty;
} reagent;

typedef struct {
    asset_id product;
    uint32_t product_qty;
    asset_id producer;
    reagent *reagents;
} recipe;

#include "lists/crafting_list.c"

uint32_t craft__find_num_recipes_by_reagent(asset_id producer, asset_id id) {
    uint32_t num_recipes=0;
    for (int i = 0; i < (int)MAX_RECIPES; ++i) {
        if (recipes[i].producer == producer) {
            for (int j = 0; recipes[i].reagents[j].id; ++j) {
                if (recipes[i].reagents[j].id == id) {
                    ++num_recipes;
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

bool craft_is_reagent_used_in_producer(asset_id reagent, asset_id producer) {
    return craft__find_num_recipes_by_reagent(producer, reagent) > 0;
}

asset_id craft_perform_recipe(ecs_entity_t *items, asset_id producer, uint32_t *quantity) {
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
            return rec->product;
        }
    }
    return 0;
}

// TODO(zaklaus): 
asset_id craft_has_byproducts(asset_id product) {
    return 0xFF;
}

// TODO(zaklaus): 
uint32_t craft_resolve_graph(asset_id product, uint16_t *hops, uint8_t direct_cost) {
    return 0;
}
