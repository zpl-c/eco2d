#include "camera.h"
#include "item_placement.h"

static bool build_submit_placements = false;
static bool build_is_in_draw_mode = false;
static uint8_t build_num_placements = 0;
static item_placement build_placements[BUILD_MAX_PLACEMENTS] = {0};

void buildmode_draw() {
    if (inv_is_open) return;
    camera cam = camera_get();
    Vector2 mpos = GetMousePosition();
    entity_view *e = game_world_view_active_get_entity(cam.ent_id);
    if (!e) return;
    float zoom = renderer_zoom_get();
    mpos.x -= screenWidth/2.0f;
    mpos.y -= screenHeight/2.0f;
    cam.x += mpos.x*(1.0f/zoom);
    cam.y += mpos.y*(1.0f/zoom);
    cam.x = ((int32_t)cam.x / (int32_t)(WORLD_BLOCK_SIZE)) * WORLD_BLOCK_SIZE;
    cam.y = ((int32_t)cam.y / (int32_t)(WORLD_BLOCK_SIZE)) * WORLD_BLOCK_SIZE;
    cam.x += WORLD_BLOCK_SIZE/2.0f;
    cam.y += WORLD_BLOCK_SIZE/2.0f;
    
    if (e->has_items && !e->inside_vehicle && e->items[e->selected_item].quantity > 0) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !build_is_in_draw_mode) {
            build_is_in_draw_mode = true;
            build_num_placements = 0;
            
            item_placement empty_placement = { .x = 0.0f, .y = 0.0f, .kind = -1 };
            for (size_t i = 0; i < BUILD_MAX_PLACEMENTS; i++) {
                zpl_memcopy(&build_placements[i], &empty_placement, zpl_size_of(item_placement));
            }
        }
        
        uint32_t qty = e->items[e->selected_item].quantity;
        
        if (build_is_in_draw_mode) {
            for (size_t i = 0; i < BUILD_MAX_PLACEMENTS; i++) {
                item_placement *it = &build_placements[i];
                if (it->kind == -1) {
                    it->x = (float)cam.x;
                    it->y = (float)cam.y;
                    it->kind = 0;
                    build_num_placements++;
                    break;
                } else if (it->x == (float)cam.x && it->y == (float)cam.y) {
                    break;
                }
            }
            
            build_num_placements = zpl_min(build_num_placements, qty);
            
            for (size_t i = 0; i < build_num_placements; i++) {
                item_placement *it = &build_placements[i];
                renderer_draw_single(it->x, it->y, ASSET_DEBUG_TILE, ColorAlpha(BLUE, 0.4f));
            }
            
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                build_is_in_draw_mode = false;
            }
            
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                build_submit_placements = true;
                build_is_in_draw_mode = false;
            }
        }
        
        renderer_draw_single(cam.x, cam.y, ASSET_DEBUG_TILE, ColorAlpha(BLUE, 0.4f));
    }
}