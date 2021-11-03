#include "camera.h"
#include "item_placement.h"

#define ZPL_ENABLE_MATH
#include "zpl.h"

static bool build_submit_placements = false;
static bool build_is_in_draw_mode = false;
static uint8_t build_num_placements = 0;
static item_placement build_placements[BUILD_MAX_PLACEMENTS] = {0};

void buildmode_clear_buffers(void) {
    item_placement empty_placement = { .x = 0.0f, .y = 0.0f, .kind = -1 };
    for (size_t i = 0; i < BUILD_MAX_PLACEMENTS; i++) {
        zpl_memcopy(&build_placements[i], &empty_placement, zpl_size_of(item_placement));
    }
}

void buildmode_draw(void) {
    camera cam = camera_get();
    camera old_cam = cam;
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
    
    // NOTE(zaklaus): Check distance
    double dx = old_cam.x - cam.x;
    double dy = old_cam.y - cam.y;
    double dsq = (dx*dx + dy*dy);
    bool is_outside_range = (dsq > zpl_square(WORLD_BLOCK_SIZE*8));
    
    if (build_submit_placements) {
        build_submit_placements = false;
        buildmode_clear_buffers();
    }
    
    ItemDrop *item = &e->items[e->selected_item];
    
    if (e->has_items && !e->inside_vehicle && item->quantity > 0 && !is_outside_range) {
        uint16_t item_id = item_find(item->kind);
        item_usage usage = item_get_usage(item_id);
        if (usage < UKIND_END_PLACE) {
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !build_is_in_draw_mode) {
                build_is_in_draw_mode = true;
                build_num_placements = 0;
                buildmode_clear_buffers();
            }
            
            uint32_t qty = item->quantity;
            bool directional = item_get_place_directional(item_id);
            
            if (build_is_in_draw_mode) {
                for (size_t i = 0; i < BUILD_MAX_PLACEMENTS; i++) {
                    item_placement *it = &build_placements[i];
                    if (it->kind == -1) {
                        if (directional && build_num_placements > 2) {
                            float p1x = build_placements[0].x;
                            float p1y = build_placements[0].y;
                            float p2x = build_placements[1].x;
                            float p2y = build_placements[1].y;
                            float p3x = (float)cam.x;
                            float p3y = (float)cam.y;
                            float sx = zpl_sign(p2x-p1x);
                            float sy = zpl_sign(p2y-p1y);
                            float sxx = zpl_sign(p3x-p1x);
                            float syy = zpl_sign(p3y-p1y);
                            
                            if (sx != sxx || sy != syy) break;
                        }
                        it->x = (float)cam.x;
                        it->y = (float)cam.y;
                        it->kind = 0;
                        build_num_placements++;
                        break;
                    } else if (it->x == (float)cam.x && it->y == (float)cam.y) {
                        break;
                    }
                }
                
            }
            
            if (!is_outside_range)
                renderer_draw_single(cam.x, cam.y, ASSET_EMPTY, ColorAlpha(BLUE, 0.4f));
            
            build_num_placements = zpl_min(build_num_placements, qty);
        }
    }
    
    for (size_t i = 0; i < build_num_placements; i++) {
        item_placement *it = &build_placements[i];
        renderer_draw_single(it->x, it->y, ASSET_EMPTY, ColorAlpha(BLUE, 0.4f));
    }
    
    if (build_is_in_draw_mode) {
        if (IsKeyPressed(KEY_SPACE)) {
            build_is_in_draw_mode = false;
        }
        
        if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            build_submit_placements = true;
            build_is_in_draw_mode = false;
        }
    }
    
}