#include "core/camera.h"
#include "models/item_placement.h"

static bool build_submit_placements = false;
static bool build_is_in_draw_mode = false;
extern bool inv_is_inside;

static uint8_t build_num_placements = 0;
static item_placement build_placements[BUILD_MAX_PLACEMENTS] = {0};
bool build_is_deletion_mode = false;

void buildmode_clear_buffers(void) {
    item_placement empty_placement = { .x = 0.0f, .y = 0.0f, .kind = -1 };
    for (size_t i = 0; i < BUILD_MAX_PLACEMENTS; i++) {
        zpl_memcopy(&build_placements[i], &empty_placement, zpl_size_of(item_placement));
    }
}

void buildmode_draw(void) {
    if (inv_is_inside) return;
    camera cam = camera_get();
    camera old_cam = cam;
    entity_view *e = game_world_view_active_get_entity(cam.ent_id);
    if (!e) return;
    float zoom = renderer_zoom_get();
    float mx = 0, my = 0;
    platform_get_block_realpos(&mx, &my);
    cam.x = (double)mx;
    cam.y = (double)my;

    renderer_draw_single((float)cam.x, (float)cam.y, ASSET_BLOCK_FRAME, WHITE);

    // NOTE(zaklaus): Check distance
    double dx = old_cam.x - cam.x;
    double dy = old_cam.y - cam.y;
    double dsq = (dx*dx + dy*dy);
    bool is_outside_range = (dsq > zpl_square(WORLD_BLOCK_SIZE*14));

    if (build_submit_placements) {
        build_submit_placements = false;
        buildmode_clear_buffers();
    }

    if (IsKeyPressed(KEY_B)){
        build_is_deletion_mode = !build_is_deletion_mode;
    }

    Item *item = &e->items[e->selected_item];

    if (e->has_items && !e->inside_vehicle && (build_is_deletion_mode || (item->quantity > 0 && !is_outside_range))) {
        item_usage usage = 0;
        uint16_t item_id = 0;
        if (!build_is_deletion_mode){
            item_id = item_find(item->kind);
            usage = item_get_usage(item_id);
        }
        if (usage < UKIND_END_PLACE) {
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !build_is_in_draw_mode) {
                build_is_in_draw_mode = true;
                build_num_placements = 0;
                buildmode_clear_buffers();
            }

            uint32_t qty = BUILD_MAX_PLACEMENTS;
            bool directional = false;

            if (!build_is_deletion_mode){
                directional = item_get_place_directional(item_id);
                qty = item->quantity;
            }

            world_block_lookup l = world_block_from_realpos((float)cam.x, (float)cam.y);
            if (build_is_deletion_mode && !l.is_outer){
                renderer_draw_single((float)cam.x, (float)cam.y, ASSET_BUILDMODE_HIGHLIGHT, ColorAlpha(RED, 0.4f));
                goto build_skip_placements;
            }

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
                            float sx = zpl_sign0(p2x-p1x);
                            float sy = zpl_sign0(p2y-p1y);
                            float sxx = zpl_sign0(p3x-p1x);
                            float syy = zpl_sign0(p3y-p1y);

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


            if (!is_outside_range) {
                if (build_is_deletion_mode)
                    renderer_draw_single((float)cam.x, (float)cam.y, ASSET_BUILDMODE_HIGHLIGHT, ColorAlpha(RED, 0.2f));
                else
                    renderer_draw_single((float)cam.x, (float)cam.y, item->kind, ColorAlpha(WHITE, 0.2f));
            }

            build_skip_placements:
            build_num_placements = zpl_min(build_num_placements, qty);
        }
    }

    for (size_t i = 0; i < build_num_placements; i++) {
        item_placement *it = &build_placements[i];
        renderer_draw_single(it->x, it->y, !build_is_deletion_mode ? item->kind : ASSET_BUILDMODE_HIGHLIGHT, ColorAlpha(build_is_deletion_mode ? RED : RAYWHITE, 0.6f));
    }

    if (build_is_in_draw_mode) {
        if (IsKeyPressed(KEY_SPACE)) {
            build_is_in_draw_mode = false;
            buildmode_clear_buffers();
        }

        if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            build_submit_placements = true;
            build_is_in_draw_mode = false;
        }
    }

}
