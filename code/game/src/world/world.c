#include "zpl.h"
#include "librg.h"
#include "modules/components.h"
#include "modules/systems.h"
#include "world/world.h"
#include "entity_view.h"
#include "debug_replay.h"
#include "world/worldgen/worldgen.h"
#include "platform.h"
#include "profiler.h"

#include "packets/pkt_send_librg_update.h"

ZPL_TABLE(static, world_snapshot, world_snapshot_, entity_view);

static world_data world = {0};
static world_snapshot streamer_snapshot;

entity_view world_build_entity_view(int64_t e) {
    entity_view *cached_ev = world_snapshot_get(&streamer_snapshot, e);
    if (cached_ev) return *cached_ev;
    
    entity_view view = {0};
    
    const Classify *classify = ecs_get(world_ecs(), e, Classify);
    ZPL_ASSERT(classify);
    
    view.kind = classify->id;
    
    const Position *pos = ecs_get(world_ecs(), e, Position);
    if (pos) {
        view.x = pos->x;
        view.y = pos->y;
    }
    
    const Velocity *vel = ecs_get(world_ecs(), e, Velocity);
    if (vel) {
        view.flag |= EFLAG_INTERP;
        view.vx = vel->x;
        view.vy = vel->y;
    }
    
    const Health *health = ecs_get(world_ecs(), e, Health);
    if (health) {
        view.hp = health->hp;
        view.max_hp = health->max_hp;
    }
    
    if (ecs_get(world_ecs(), e, Vehicle)) {
        Vehicle const* veh = ecs_get(world_ecs(), e, Vehicle);
        view.heading = veh->heading;
    }
    
    Chunk *chpos = 0;
    if ((chpos = ecs_get_mut_if(world_ecs(), e, Chunk))) {
        view.x = chpos->x;
        view.y = chpos->y;
        view.blocks_used = 1;
        view.is_dirty = chpos->is_dirty;
        chpos->is_dirty = false;
        
        for (int i = 0; i < world.chunk_size*world.chunk_size; i += 1) {
            view.blocks[i] = world.block_mapping[chpos->id][i];
        }
    }
    
    world_snapshot_set(&streamer_snapshot, e, view);
    return view;
}

int32_t tracker_write_create(librg_world *w, librg_event *e) {
    int64_t entity_id = librg_event_entity_get(w, e);
#ifdef WORLD_LAYERING
    if (world.active_layer_id != WORLD_TRACKER_LAYERS-1) {
        // NOTE(zaklaus): reject updates from smaller layers
        return LIBRG_WRITE_REJECT;
    }
#endif
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);
    
    return (int32_t)entity_view_pack_struct(buffer, actual_length, world_build_entity_view(entity_id));
}

int32_t tracker_write_remove(librg_world *w, librg_event *e) {
    (void)e;
    (void)w;
#ifdef WORLD_LAYERING
    if (world.active_layer_id != WORLD_TRACKER_LAYERS-1) {
        // NOTE(zaklaus): reject updates from smaller layers
        return LIBRG_WRITE_REJECT;
    }
#endif
    return 0;
}

int32_t tracker_write_update(librg_world *w, librg_event *e) {
    int64_t entity_id = librg_event_entity_get(w, e);
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);
    entity_view view = world_build_entity_view(entity_id);
    
    // NOTE(zaklaus): exclude chunks from updates as they never move
    {
        if (view.kind == EKIND_CHUNK && !view.is_dirty) {
            return LIBRG_WRITE_REJECT;
        }
    }
    
    return (int32_t)entity_view_pack_struct(buffer, actual_length, view);
}

void world_setup_pkt_handlers(world_pkt_reader_proc *reader_proc, world_pkt_writer_proc *writer_proc) {
    world.reader_proc = reader_proc;
    world.writer_proc = writer_proc;
}

int32_t world_init(int32_t seed, uint16_t chunk_size, uint16_t chunk_amount) {
    if (world.data) {
        return 0;
    }
    
    world.seed = seed;
    world.chunk_size = chunk_size;
    world.chunk_amount = chunk_amount;
    
    world.dim = (world.chunk_size * world.chunk_amount);
    world.size = world.dim * world.dim;
    
    if (world.tracker == NULL) {
        world.tracker = librg_world_create();
    }
    
    if (world.tracker == NULL) {
        zpl_printf("[ERROR] An error occurred while trying to create a server world.\n");
        return WORLD_ERROR_TRACKER_FAILED;
    }
    
    /* config our world grid */
    librg_config_chunksize_set(world.tracker, WORLD_BLOCK_SIZE * world.chunk_size, WORLD_BLOCK_SIZE * world.chunk_size, 0);
    librg_config_chunkamount_set(world.tracker, world.chunk_amount, world.chunk_amount, 0);
    librg_config_chunkoffset_set(world.tracker, LIBRG_OFFSET_BEG, LIBRG_OFFSET_BEG, LIBRG_OFFSET_BEG);
    
    librg_event_set(world.tracker, LIBRG_WRITE_CREATE, tracker_write_create);
    librg_event_set(world.tracker, LIBRG_WRITE_REMOVE, tracker_write_remove);
    librg_event_set(world.tracker, LIBRG_WRITE_UPDATE, tracker_write_update);
    
    world.data = zpl_malloc(sizeof(uint8_t)*world.size);
    
    if (!world.data) {
        return WORLD_ERROR_OUTOFMEM;
    }
    
    world.ecs = ecs_init();
    
    ECS_IMPORT(world.ecs, Components);
    ECS_IMPORT(world.ecs, Systems);
    world.ecs_update = ecs_query_new(world.ecs, "components.ClientInfo, components.Position");
    world.chunk_mapping = zpl_malloc(sizeof(ecs_entity_t)*zpl_square(chunk_amount));
    world.block_mapping = zpl_malloc(sizeof(uint8_t*)*zpl_square(chunk_amount));
    world_snapshot_init(&streamer_snapshot, zpl_heap());
    
    int32_t world_build_status = worldgen_test(&world);
    ZPL_ASSERT(world_build_status >= 0);
    
    for (int i = 0; i < world.chunk_amount * world.chunk_amount; ++i) {
        ecs_entity_t e = ecs_new(world.ecs, 0);
        ecs_set(world.ecs, e, Classify, {.id = EKIND_CHUNK });
        Chunk *chunk = ecs_get_mut(world.ecs, e, Chunk, NULL);
        librg_entity_track(world.tracker, e);
        librg_entity_chunk_set(world.tracker, e, i);
        librg_chunk_to_chunkpos(world.tracker, i, &chunk->x, &chunk->y, NULL);
        world.chunk_mapping[i] = e;
        world.block_mapping[i] = zpl_malloc(sizeof(uint8_t)*zpl_square(chunk_size));
        chunk->id = i;
        chunk->is_dirty = false;
        
        for (int y = 0; y < chunk_size; y += 1) {
            for (int x = 0; x < chunk_size; x += 1) {
                int chk_x = chunk->x * chunk_size;
                int chk_y = chunk->y * chunk_size;
                uint8_t *c = &world.block_mapping[i][(y*chunk_size)+x];
                *c = world.data[(chk_y+y)*world.dim + (chk_x+x)];
            }
        }
    }
    
    zpl_printf("[INFO] Created a new server world\n");
    
    return world_build_status;
}

int32_t world_destroy(void) {
    librg_world_destroy(world.tracker);
    ecs_fini(world.ecs);
    zpl_mfree(world.data);
    zpl_mfree(world.chunk_mapping);
    for (int i = 0; i < zpl_square(world.chunk_amount); i+=1) {
        zpl_mfree(world.block_mapping[i]);
    }
    zpl_mfree(world.block_mapping);
    world_snapshot_destroy(&streamer_snapshot);
    zpl_memset(&world, 0, sizeof(world));
    zpl_printf("[INFO] World was destroyed.\n");
    return WORLD_ERROR_NONE;
}

#define WORLD_LIBRG_BUFSIZ 2000000

static void world_tracker_update(uint8_t ticker, uint32_t freq, uint8_t radius) {
    if (world.tracker_update[ticker] > zpl_time_rel_ms()) return;
    world.tracker_update[ticker] = zpl_time_rel_ms() + freq;
    
    profile(PROF_WORLD_WRITE) {
        ecs_iter_t it = ecs_query_iter(world.ecs_update);
        static char buffer[WORLD_LIBRG_BUFSIZ] = {0};
        world.active_layer_id = ticker;
        
        while (ecs_query_next(&it)) {
            ClientInfo *p = ecs_column(&it, ClientInfo, 1);
            
            for (int i = 0; i < it.count; i++) {
                size_t datalen = WORLD_LIBRG_BUFSIZ;
                
                // TODO(zaklaus): SUPER TEMPORARY HOT !!! simulate variable radius queries
                {
                    librg_entity_radius_set(world_tracker(), p[i].peer, radius);
                }
                // TODO(zaklaus): push radius once librg patch comes in
                int32_t result = librg_world_write(world_tracker(), p[i].peer, buffer, &datalen, NULL);
                
                if (result > 0) {
                    zpl_printf("[info] buffer size was not enough, please increase it by at least: %d\n", result);
                } else if (result < 0) {
                    zpl_printf("[error] an error happened writing the world %d\n", result);
                }
                
                pkt_send_librg_update((uint64_t)p[i].peer, p[i].view_id, ticker, buffer, datalen);
            }
        }
        
        // NOTE(zaklaus): clear out our streaming snapshot
        // TODO(zaklaus): move this to zpl
        {
            zpl_array_clear(streamer_snapshot.hashes);
            zpl_array_clear(streamer_snapshot.entries);
        }
    }
}


int32_t world_update() {
    profile (PROF_UPDATE_SYSTEMS) {
        ecs_progress(world.ecs, 0.0f);
    }
    
    world_tracker_update(0, WORLD_TRACKER_UPDATE_FAST_MS, 2);
    world_tracker_update(1, WORLD_TRACKER_UPDATE_NORMAL_MS, 4);
    world_tracker_update(2, WORLD_TRACKER_UPDATE_SLOW_MS, 6);
    
    debug_replay_update();
    return 0;
}

int32_t world_read(void* data, uint32_t datalen, void *udata) {
    if (world.reader_proc) {
        return world.reader_proc(data, datalen, udata);
    }
    return -1;
}

int32_t world_write(pkt_header *pkt, void *udata) {
    if (world.writer_proc) {
        return world.writer_proc(pkt, udata);
    }
    return -1;
}

uint32_t world_buf(uint8_t const **ptr, uint32_t *width) {
    ZPL_ASSERT_NOT_NULL(world.data);
    ZPL_ASSERT_NOT_NULL(ptr);
    *ptr = world.data;
    if (width) *width = world.dim;
    return world.size;
}

ecs_world_t * world_ecs() {
    return world.ecs;
}

librg_world *world_tracker() {
    return world.tracker;
}

uint16_t world_chunk_size(void) {
    return world.chunk_size;
}

uint16_t world_chunk_amount(void) {
    return world.chunk_amount;
}

uint16_t world_dim(void) {
    return WORLD_BLOCK_SIZE * world.chunk_size * world.chunk_amount;
}

ecs_entity_t world_chunk_mapping(librg_chunk id) {
    ZPL_ASSERT(id >= 0 && id < zpl_square(world.chunk_amount));
    return world.chunk_mapping[id];
}

world_block_lookup world_block_from_realpos(float x, float y) {
    x = zpl_clamp(x, 0, world_dim()-1);
    y = zpl_clamp(y, 0, world_dim()-1);
    librg_chunk chunk_id = librg_chunk_from_realpos(world.tracker, x, y, 0);
    ecs_entity_t e = world.chunk_mapping[chunk_id];
    int32_t size = world.chunk_size * WORLD_BLOCK_SIZE;
    int16_t chunk_x, chunk_y;
    librg_chunk_to_chunkpos(world.tracker, chunk_id, &chunk_x, &chunk_y, NULL);
    
    // NOTE(zaklaus): pos relative to chunk
    float chx = x - chunk_x * size;
    float chy = y - chunk_y * size;
    
    uint32_t bx = (uint32_t)chx / WORLD_BLOCK_SIZE;
    uint32_t by = (uint32_t)chy / WORLD_BLOCK_SIZE;
    uint32_t block_idx = (by*world.chunk_size)+bx;
    uint8_t block_id = world.block_mapping[chunk_id][block_idx];
    
    // NOTE(zaklaus): pos relative to block's center
    float box = chx - bx * WORLD_BLOCK_SIZE - WORLD_BLOCK_SIZE/2.0f;
    float boy = chy - by * WORLD_BLOCK_SIZE - WORLD_BLOCK_SIZE/2.0f;
    
    world_block_lookup lookup = {
        .id = block_idx,
        .block_id = block_id,
        .chunk_id = chunk_id,
        .chunk_e = e,
        .ox = box,
        .oy = boy,
    };
    
    return lookup;
}

world_block_lookup world_block_from_index(int64_t id, uint16_t block_idx) {
    uint8_t block_id = world.block_mapping[id][block_idx];
    
    world_block_lookup lookup = {
        .id = block_idx,
        .block_id = block_id,
        .chunk_id = id,
        .chunk_e = world.chunk_mapping[id],
    };
    
    return lookup;
}

int64_t world_chunk_from_realpos(float x, float y) {
    librg_chunk chunk_id = librg_chunk_from_realpos(world.tracker, x, y, 0);
    return world.chunk_mapping[chunk_id];
}

int64_t world_chunk_from_entity(ecs_entity_t id) {
    return librg_entity_chunk_get(world.tracker, id);
}

void world_chunk_replace_block(ecs_world_t *ecs, int64_t id, uint16_t block_idx, uint8_t block_id) {
    ZPL_ASSERT(block_idx >= 0 && block_idx < zpl_square(world.chunk_size));
    world.block_mapping[id][block_idx] = block_id;
    world_chunk_mark_dirty(ecs, world.chunk_mapping[id]);
}

uint8_t *world_chunk_get_blocks(int64_t id) {
    return world.block_mapping[id];
}

void world_chunk_mark_dirty(ecs_world_t *ecs, ecs_entity_t e) {
    bool was_added=false;
    Chunk *chunk = ecs_get_mut(ecs, e, Chunk, &was_added);
    ZPL_ASSERT(!was_added);
    if (chunk) chunk->is_dirty = true;
}

uint8_t world_chunk_is_dirty(ecs_world_t *ecs, ecs_entity_t e) {
    bool was_added=false;
    Chunk *chunk = ecs_get_mut(ecs, e, Chunk, &was_added);
    ZPL_ASSERT(!was_added);
    if (chunk) return chunk->is_dirty;
    return false;
}

int64_t *world_chunk_fetch_entities(librg_chunk chunk_id, size_t *ents_len) {
    ZPL_ASSERT_NOT_NULL(ents_len);
    static int64_t ents[UINT16_MAX];
    *ents_len = UINT16_MAX;
    librg_world_fetch_chunk(world.tracker, chunk_id, ents, ents_len);
    return ents;
}

int64_t *world_chunk_fetch_entities_realpos(float x, float y, size_t *ents_len) {
    return world_chunk_fetch_entities(librg_chunk_from_realpos(world.tracker, x, y, 0), ents_len);
}

int64_t *world_chunk_query_entities(int64_t e, size_t *ents_len, int8_t radius) {
    ZPL_ASSERT_NOT_NULL(ents_len);
    static int64_t ents[UINT16_MAX];
    *ents_len = UINT16_MAX;
    librg_entity_radius_set(world.tracker, e, radius);
    librg_world_query(world.tracker, e, ents, ents_len);
    return ents;
}

uint8_t world_entity_valid(ecs_entity_t e) {
    if (!e) return false;
    return ecs_is_alive(world.ecs, e);
}
