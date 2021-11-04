#include "zpl.h"
#include "camera.h"
#include "platform.h"
#include "entity_view.h"
#include "game.h"

#define CAMERA_LERP_FACTOR 11.2f

static camera main_camera;

void camera_reset(void) {
    zpl_zero_item(&main_camera);
    main_camera.mode = CAMERA_MODE_STATIONARY;
    main_camera.first_time = true;
}

void camera_update(void) {
    switch (main_camera.mode) {
        case CAMERA_MODE_FOLLOW: {
            world_view *world = game_world_view_get_active();
            if (!world) break;
            entity_view *view = entity_view_get(&world->entities, main_camera.ent_id);
            if (!view) break;
            float smooth_ms = zpl_clamp((float)platform_frametime(), 0.0f, 1.0f);
            
            main_camera.x = zpl_lerp((float)main_camera.x, view->x, CAMERA_LERP_FACTOR*smooth_ms);
            main_camera.y = zpl_lerp((float)main_camera.y, view->y, CAMERA_LERP_FACTOR*smooth_ms);
            
            if (main_camera.first_time) {
                main_camera.first_time = false;
                main_camera.x = view->x;
                main_camera.y = view->y;
            }
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
