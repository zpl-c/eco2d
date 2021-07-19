#include "zpl.h"
#include "librg.h"
#include "modules/general.h"
#include "modules/net.h"
#include "modules/physics.h"
#include "world/world.h"
#include "entity_view.h"
#include "world/worldgen/worldgen.h"
#include "platform.h"
#include "profiler.h"

#include "packets/pkt_send_librg_update.h"

static world_data world = {0};

entity_view world_build_entity_view(int64_t e) {
    ECS_IMPORT(world_ecs(), General);
    ECS_IMPORT(world_ecs(), Physics);
    ECS_IMPORT(world_ecs(), Net);
    entity_view view = {0};
    
    // TODO(zaklaus): branch out based on ECS tags
    const Position *pos = ecs_get(world_ecs(), e, Position);
    if (pos) {
        view.kind = ecs_has(world_ecs(), e, EcsClient) ? EKIND_PLAYER : EKIND_THING;
        view.x = pos->x;
        view.y = pos->y;
    }
    
    const Velocity *vel = ecs_get(world_ecs(), e, Velocity);
    if (vel) {
        view.flag |= EFLAG_INTERP;
        view.vx = vel->x;
        view.vy = vel->y;
    }
    
    const Chunk *chpos = ecs_get(world_ecs(), e, Chunk);
    if (chpos) {
        view.kind = EKIND_CHUNK;
        view.x = chpos->x;
        view.y = chpos->y;
        view.blocks_used = 1;
        
        for (int i = 0; i < world.chunk_size*world.chunk_size; i += 1) {
            view.blocks[i] = *ecs_vector_get(chpos->blocks, uint8_t, i);
        }
    }
    
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
    // TODO(zaklaus): use dirty flag to send updates if chunk changes
    {
        if (view.kind == EKIND_CHUNK) {
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
    ecs_set_entity_range(world.ecs, 0, UINT32_MAX);
    
    ECS_IMPORT(world.ecs, General);
    ECS_IMPORT(world.ecs, Net);
    world.ecs_update = ecs_query_new(world.ecs, "net.ClientInfo, general.Position");
    
    int32_t world_build_status = worldgen_test(&world);
    ZPL_ASSERT(world_build_status >= 0);
    
    for (int i = 0; i < world.chunk_amount * world.chunk_amount; ++i) {
        ecs_entity_t e = ecs_new(world.ecs, 0);
        Chunk *chunk = ecs_get_mut(world.ecs, e, Chunk, NULL);
        librg_entity_track(world.tracker, e);
        librg_entity_chunk_set(world.tracker, e, i);
        librg_chunk_to_chunkpos(world.tracker, i, &chunk->x, &chunk->y, NULL);
        chunk->blocks = NULL;
        
        for (int y = 0; y < world.chunk_size; y += 1) {
            for (int x = 0; x < world.chunk_size; x += 1) {
                int chk = world.chunk_size * i;
                int chk_x = chk % world.dim;
                int chk_y = chk / world.dim;
                uint8_t *c = ecs_vector_add(&chunk->blocks, uint8_t);
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
    zpl_memset(&world, 0, sizeof(world));
    zpl_printf("[INFO] World was destroyed.\n");
    return WORLD_ERROR_NONE;
}

#define WORLD_LIBRG_BUFSIZ 2000000

static void world_tracker_update(uint8_t ticker, uint32_t freq, uint8_t radius) {
    if (world.tracker_update[ticker] > zpl_time_rel_ms()) return;
    world.tracker_update[ticker] = zpl_time_rel_ms() + freq;
    
    profile(PROF_WORLD_WRITE) {
        ECS_IMPORT(world.ecs, General);
        ECS_IMPORT(world.ecs, Net);
        
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
    }
}


int32_t world_update() {
    profile (PROF_UPDATE_SYSTEMS) {
        ecs_progress(world.ecs, 0.0f);
    }
    
    world_tracker_update(0, WORLD_TRACKER_UPDATE_FAST_MS, 1);
    world_tracker_update(1, WORLD_TRACKER_UPDATE_NORMAL_MS, 2);
    world_tracker_update(2, WORLD_TRACKER_UPDATE_SLOW_MS, 3);
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

librg_world * world_tracker() {
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
