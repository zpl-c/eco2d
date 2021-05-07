#include "zpl.h"
#include "camera.h"
#include "entity_view.h"
#include "game.h"

#define CAMERA_LERP_FACTOR 0.13

static camera main_camera;

void camera_reset(void) {
    zpl_zero_item(&main_camera);
    main_camera.mode = CAMERA_MODE_STATIONARY;
}

void camera_update(void) {
    switch (main_camera.mode) {
        case CAMERA_MODE_FOLLOW: {
            world_view *world = game_world_view_get_active();
            if (!world) break;
            entity_view *view = entity_view_get(&world->entities, main_camera.ent_id);
            if (!view) break;
            
            main_camera.x = zpl_lerp(main_camera.x, view->x, CAMERA_LERP_FACTOR);
            main_camera.y = zpl_lerp(main_camera.y, view->y, CAMERA_LERP_FACTOR);
        }break;
        
        default: {
            
        }break;
    }
}
void camera_set_follow(uint64_t ent_id) {
    main_camera.mode = CAMERA_MODE_FOLLOW;
    main_camera.ent_id = ent_id;
}
void camera_set_pos(double x, double y) {
    main_camera.mode = CAMERA_MODE_STATIONARY;
    main_camera.x = x;
    main_camera.y = y;
}
camera camera_get(void) {
    return main_camera;
}
