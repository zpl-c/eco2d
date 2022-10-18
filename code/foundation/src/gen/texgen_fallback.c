#include "gen/texgen.h"
#include "world/world.h"
#include "zpl.h"
#include "utils/raylib_helpers.h"

Texture2D texgen_build_anim_fallback(asset_id id, int64_t counter) {
    (void)counter;
    switch (id) {
        case ASSET_WATER: {
            return LoadTexEco(zpl_bprintf("%s%d", "water", counter%3));
        }break;
        
        default: return GenColorEco(PINK); break;
    }
}

Texture2D texgen_build_sprite_fallback(asset_id id) {
    switch (id) {
        case ASSET_BLANK: return GenColorEco(WHITE); break;
        case ASSET_BUILDMODE_HIGHLIGHT: return GenColorEco(WHITE); break;
        case ASSET_BLOCK_FRAME: return GenFrameRect(); break;
        
        // NOTE(zaklaus): items
        case ASSET_COAL: return LoadTexEco("coal");
        case ASSET_IRON_ORE: return LoadTexEco("iron_ore");
        //case ASSET_IRON_INGOT: return LoadTexEco("iron_ingot");
        case ASSET_IRON_PLATES: return LoadTexEco("iron_plate");
        case ASSET_SCREWS: return LoadTexEco("screws");
        case ASSET_LOG: return LoadTexEco("log");
        case ASSET_PLANK: return LoadTexEco("plank");
        
        // NOTE(zaklaus): blocks
        case ASSET_FENCE: return LoadTexEco("fence");
        case ASSET_GROUND: return LoadTexEco("grass");
        case ASSET_DIRT: return LoadTexEco("dirt");
        case ASSET_WALL: return LoadTexEco("asphalt");
        case ASSET_HILL_SNOW:
        case ASSET_HILL: return LoadTexEco("rock");
        case ASSET_LAVA: return LoadTexEco("lava");
        case ASSET_WOOD: return LoadTexEco("wood");
        case ASSET_TREE: return LoadTexEco("bigtree");
        case ASSET_TEST_TALL: return LoadTexEco("test-tall");
        // case ASSET_WATER: return LoadTexEco("water");
        
        case ASSET_BELT:
        case ASSET_BELT_RIGHT: return LoadTexEco("belt_right");
        case ASSET_BELT_LEFT: return LoadTexEco("belt_left");
        case ASSET_BELT_UP: return LoadTexEco("belt_up");
        case ASSET_BELT_DOWN: return LoadTexEco("belt_down");
        
        // NOTE(zaklaus): devices
        case ASSET_CHEST: return LoadTexEco("chest");
        case ASSET_FURNACE: return LoadTexEco("furnace-export");
        case ASSET_CRAFTBENCH: return LoadTexEco("craftbench");
        
        default: break;
    }
    
    if (id > ASSET_BLUEPRINT_BEGIN && id < ASSET_BLUEPRINT_END) {
        return LoadTexEco("blueprint");
    }
    
    return GenColorEco(PINK);
}
