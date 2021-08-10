#include "debug_replay.h"
#include "camera.h"
#include "entity.h"

typedef struct {
    pkt_send_keystate pkt;
    uint64_t delay;
} replay_record;

static uint8_t is_recording = false;
static replay_record *records = NULL;
static uint64_t last_record_time = 0.0f;

static uint8_t is_playing = false;
static int record_pos = 0;
static uint64_t playback_time = 0;
static ecs_entity_t mime = 0;
static ecs_entity_t plr = 0;

void debug_replay_start(void) {
    is_recording = true;
    
    if (records) zpl_array_free(records);
    zpl_array_init(records, zpl_heap());
    
    last_record_time = zpl_time_rel_ms();
}

void debug_replay_clear(void) {
    if (!records || is_playing || is_recording) return;
    zpl_array_free(records);
    records = NULL;
}

void debug_replay_stop(void) {
    is_recording = false;
    // TODO(zaklaus): 
}

void debug_replay_run(void) {
    if (mime) return;
    is_playing = true;
    record_pos = 0;
    playback_time = zpl_time_rel_ms();
    
    plr = camera_get().ent_id;
    Position const *p1 = ecs_get(world_ecs(), plr, Position);
    
    mime = entity_spawn(EKIND_DEMO_NPC);
    Position *pos = ecs_get_mut(world_ecs(), mime, Position, NULL);
    *pos = *p1;
    
    ecs_set(world_ecs(), mime, Input, {0});
    
    camera_set_follow(mime);
}

void debug_replay_update(void) {
    if (!is_playing) return;
    if (playback_time >= zpl_time_rel_ms()) return;
    
    replay_record *r = &records[record_pos];
    playback_time = zpl_time_rel() + r->delay;
    
    Input *i = ecs_get_mut(world_ecs(), mime, Input, NULL);
    i->x = r->pkt.x;
    i->y = r->pkt.y;
    i->use = r->pkt.use;
    i->sprint = r->pkt.sprint;
    
    record_pos += 1;
    
    // NOTE(zaklaus): remove our dummy art exhibist
    if (mime && record_pos == zpl_array_count(records)) {
        entity_despawn(mime);
        mime = 0;
        
        is_playing = false;
        camera_set_follow(plr);
    }
}

void debug_replay_record_keystate(pkt_send_keystate state) {
    if (!is_recording) return;
    float record_time = zpl_time_rel_ms();
    
    replay_record rec = {
        .pkt = state,
        .delay = (record_time - last_record_time),
    };
    
    zpl_array_append(records, rec);
    last_record_time = zpl_time_rel_ms();
}
